# -*- coding: utf-8 -*-

import asyncio

from collections import namedtuple, deque
from very_simple_xml.wrapped import parse_impl


class ParseCallback:

    class Decoder:
        def __str__(self):
            return self.__bytes__().decode('utf-8')

    class Element(
            namedtuple('element', ['name', 'attributes', 'children']),
            Decoder,
    ):
        def __new__(cls, name):
            return super().__new__(cls, name, [], [])

        def __bytes__(self):
            atts = b' '.join([bytes(a) for a in self.attributes])
            children = b''.join([bytes(c) for c in self.children])
            template = b''

            if atts:
                atts = b' ' + atts
            if children:
                template = b'<%(name)s%(atts)s>%(children)s</%(name)s>'
            else:
                template = b'<%(name)s/>%(atts)s%(children)s'
            return template % {
                b'name': self.name,
                b'atts': atts,
                b'children': children,
            }

    class Attribute(namedtuple('attribute', ['name', 'value']), Decoder):
        def __bytes__(self):
            return b'%s=%s' % (self.name, self.value)

    class Comment(Decoder, bytes):
        def __bytes__(self):
            return b'<!--' + self + b'-->'

    class CData(Decoder, bytes):
        def __bytes__(self):
            return b'<![CDATA[' + self + b']]>'

    class PI(Decoder, bytes):
        def __bytes__(self):
            return b'<?' + self + b'?>'

    node_constructors = {
        b'element': Element,
        b'document': namedtuple('document', ['children']),
        b'text': bytes,
        b'cdata': CData,
        b'comment': Comment,
        b'pi': PI,
        b'attribute': Attribute,
    }

    def __init__(self, xml_file, debug=False):
        self._xml_file = xml_file
        self._debug = False
        # self._debug = debug
        self._document = self.node_constructors[b'document']([])
        self._stack = deque()
        self._stack.append(self._document)
        self._element = self._document

    def xml_file(self):
        return self._xml_file

    def debug(self):
        return int(self._debug)

    def on_node(self, ptype, ptext, line, coll):
        elt = self.node_constructors[ptype](ptext)
        print('on_node', ptype, ptext, self._element, '->', elt)
        self._element.children.append(elt)
        if ptype == b'element':
            self._stack.append(elt)
            self._element = elt
        return True

    def on_attribute(self, pname, pvalue, line, col):
        print('on_attribute', pname, pvalue)
        att = self.node_constructors[b'attribute'](pname, pvalue)
        self._element.attributes.append(att)
        return True

    def on_pop(self):
        print('on_pop')
        self._element = self._stack.pop()
        self._element = self._stack.pop()
        self._stack.append(self._element)
        print('element', self._element)

    def __str__(self):
        children = self._document.children
        if children:
            return '\n'.join(str(c) for c in children)
        return 'Not parsed'


async def parse_files(parse_callbacks):
    await asyncio.gather(*[parse_impl(c) for c in parse_callbacks])
