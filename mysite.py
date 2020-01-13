import eventlet, random, os
from eventlet import wsgi, websocket, tpool, greenthread

@websocket.WebSocketWSGI
def startTimer(ws):
    n_cnt = 0
    while True:
        print('Timer fired! {}'.format(n_cnt))
        """ waiting for messages
        m = ws.wait()
        if m is None:
            break
        """
        #while True:
        ws.send('Timer fired! {}'.format(n_cnt))
        greenthread.sleep(2)
        n_cnt+=1

"""
@websocket.WebSocketWSGI
def handle(ws):
    if ws.path == '/echo':
        print('WS == \'/echo\'')
        while True:
            m = ws.wait()
            if m is None:
                break
            ws.send(m)
    elif ws.path == '/data':
        print('WS == \'/data\'')
        for i in range (10):
            ws.send('0 {} {}\n'.format(i, random.random()))
            eventlet.sleep(0.1)
"""
@websocket.WebSocketWSGI
def saveData(ws):
    filename = ws.wait()
    print('Filename: {}'.format(filename))
    data = ws.wait()
    data_size = float(len(data)) / 1000 #kb
    print('Sizeof data: {:.1f} kb'.format(data_size))
    new_file = os.path.join(os.path.expanduser('~'), filename)
    print('Upload saved to: {}'.format(new_file))
    with open(new_file, 'wb') as file:
        file.write(data)


#standard server
def dispatch(environ, start_response):

    """
        WEBSOCKETS
    """

    if environ['PATH_INFO'] == '/data':
        print('PATH_INFO == \'/data\'')
        return saveData(environ, start_response)
    elif environ['PATH_INFO'] == '/echo':
        print('PATH_INFO == \'/echo\'')
        return handle(environ, start_response)
    elif environ['PATH_INFO'] == '/timer':
        print('PATH_INFO == \'/timer\'')
        tpool.execute(startTimer, environ, start_response)
        return

        """
            STANDARD HTML ENDPOINTS
        """

    elif environ['PATH_INFO'] == '/':
        print('PATH_INFO == \'/\'')
        start_response('200 OK', [('content-type', 'text/html')])
        return [open(os.path.join(os.path.dirname(__file__),
            'mysite/templates/PythonicWeb.html')).read()]
   
    elif environ['PATH_INFO'] == '/qtloader.js':
        print('PATH_INFO == \'/qtloader.js\'')
        str_data = open(os.path.join(os.path.dirname(__file__),
            'mysite/static/qtloader.js')).read() 
        start_response('200 OK', [('content-type', 'application/javascript') ])

        return [str_data]

    elif environ['PATH_INFO'] == '/qtlogo.svg':
        print('PATH_INFO == \'/qtlogo.svg\'')
        img_data = open(os.path.join(os.path.dirname(__file__),
            'mysite/static/qtlogo.svg'), 'rb').read() 
        start_response('200 OK', [('content-type', 'image/svg+xml'),
                                ('content-length', str(len(img_data)))])

        return [img_data]

    elif environ['PATH_INFO'] == '/PythonicWeb.js':
        print('PATH_INFO == \'/PythonicWeb.js\'')
        str_data = open(os.path.join(os.path.dirname(__file__),
            'mysite/static/PythonicWeb.js')).read() 
        #print('java script length: {}'.format(len(str_data)))
        start_response('200 OK', [('content-type', 'application/javascript')])
        return [str_data]

    elif environ['PATH_INFO'] == '/PythonicWeb.wasm':
        print('PATH_INFO == \'/PythonicWeb.wasm\'')
        bin_data = open(os.path.join(os.path.dirname(__file__),
            'mysite/static/PythonicWeb.wasm'), 'rb').read() 
        start_response('200 OK', [('content-type', 'application/wasm')])
        return [bin_data]		

    else:
        path_info = environ['PATH_INFO']
        print('PATH_INFO = {}'.format(path_info))
        return None
		

if __name__ == '__main__':
    listener = eventlet.listen(('127.0.0.1', 7000))
    print('\nVisit http://localhost:7000/ in your websocket-capable browser.\n')
    wsgi.server(listener, dispatch)

