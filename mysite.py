import eventlet, random, os
from eventlet import wsgi, websocket

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

#standard server
def dispatch(environ, start_response):
    if environ['PATH_INFO'] == '/data':
        print('PATH_INFO == \'/data\'')
        return handle(environ, start_response)

    
    elif environ['PATH_INFO'] == '/qtloader.js':
        print('PATH_INFO == \'/qtloader.js\'')
        str_data = open(os.path.join(os.path.dirname(__file__),
            'myproject/static/qtloader.js')).read() 
        print('java script length: {}'.format(len(str_data)))
        #start_response('200 OK', [('content-type', 'application/javascript'),
        #                        ('content-length', str(len(str_data)))])
        start_response('200 OK', [('content-type', 'application/javascript') ])

        return [str_data]
        #return('Hello')


    elif environ['PATH_INFO'] == '/qtlogo.svg':
        print('PATH_INFO == \'/qtlogo.svg\'')
        bin_data = open(os.path.join(os.path.dirname(__file__),
            'myproject/static/qtlogo.svg'), 'rb').read() 
        """
        start_response('200 OK', [('content-type', 'image/png'),
                                ('content-length', str(len(bin_data)))])
        """
        start_response('200 OK', [('content-type', 'image/svg+xml'),
                                ('content-length', str(len(bin_data)))])

        return [bin_data]

    elif environ['PATH_INFO'] == '/PythonicWeb.js':
        print('PATH_INFO == \'/PythonicWeb.js\'')
        str_data = open(os.path.join(os.path.dirname(__file__),
            'myproject/static/PythonicWeb.js')).read() 
        print('java script length: {}'.format(len(str_data)))

        start_response('200 OK', [('content-type', 'application/javascript')])
        return [str_data]

    elif environ['PATH_INFO'] == '/PythonicWeb.wasm':
        print('PATH_INFO == \'/PythonicWeb.wasm\'')
        bin_data = open(os.path.join(os.path.dirname(__file__),
            'myproject/static/PythonicWeb.wasm'), 'rb').read() 
        start_response('200 OK', [('content-type', 'application/wasm')])
        return [bin_data]		

    elif environ['PATH_INFO'] == '/':
        print('PATH_INFO == \'/wasm\'')
        start_response('200 OK', [('content-type', 'text/html')])
        return [open(os.path.join(os.path.dirname(__file__),
            'myproject/templates/PythonicWeb.html')).read()]

    else:
        path_info = environ['PATH_INFO']
        print('PATH_INFO = {}'.format(path_info))
        return None
		

if __name__ == '__main__':
    listener = eventlet.listen(('127.0.0.1', 7000))
    print('\nVisit http://localhost:7000/ in your websocket-capable browser.\n')
    wsgi.server(listener, dispatch)

