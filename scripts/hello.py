#!/usr/bin/env python

import os
import urlparse

print 'Content-Type: text/html\r\n'
print '\r\n'

query = urlparse.parse_qs(os.environ.get('QUERY_STRING', ''))

print '''
<form>
<b>Name:</b><input type="text" name="name" value="{}">
<input type="submit" value="Submit">
</form>
'''.format(query.get('name', [''])[0])

if 'name' in query:
    print '<h1>Hello, {}</h1>'.format(query['name'][0])
