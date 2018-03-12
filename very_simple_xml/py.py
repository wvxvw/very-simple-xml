# -*- coding: utf-8 -*-

import asyncio

from collections import namedtuple, deque
from very_simple_xml.wrapped import parse_one


class ParseCallback:

    class Element(namedtuple('element', ['name', 'attributes', 'children'])):
        def __new__(self, name):
            self.name = name
            self.attributes = []
            self.children = []

        def __str__(self):
            print('processing xml element')
            atts = ' '.join([str(a) for a in self.attributes])
            children = '  '.join([str(c) + '\n' for c in self.children])
            template = ''

            if atts:
                atts = ' ' + atts
            if children:
                children = '  ' + children
                template = '<{:0}{:1}>{:2}</{:0}>'
            else:
                template = '<{:0}/>{:1}{:2}'
            return template.format(self.name, atts, children)

    class Attribute(namedtuple('attribute', ['name', 'value'])):
        def __str__(self):
            return '{}={!r}'.format(self.name, self.value)

    node_constructors = {
        'element': Element,
        'document': namedtuple('document', ['children']),
        'text': str,
        'cdata': str,
        'comment': str,
        'pi': str,
        'attribute': Attribute,
    }

    def __init__(self, xml_file, debug=False):
        self._xml_file = xml_file
        self._debug = False
        # self._debug = debug
        self._document = self.node_constructors['document']([])
        self._stack = deque()
        self._stack.append(self._document)
        self._element = self._document

    def xml_file(self):
        return self._xml_file

    def debug(self):
        return int(self._debug)

    def on_node(self, ptype, ptext, line, coll):
        print('on_node')
        elt = self.node_constructors[ptype](ptext)
        self._stack.append(elt)
        self._element.children.append(elt)
        if ptype == 'element':
            self._element = elt

    def on_attribute(self, pname, pvalue, line, col):
        print('on_attribute')
        att = self.node_constructors['attribute'](pname, pvalue)
        self._element.attributes.append(att)

    def on_pop(self):
        print('on_pop')
        self._stack.pop()
        self._element = self._stack.pop()
        self._stack.append(self._element)

    def __str__(self):
        if self._document.children:
            return str(self._document.children[0])
        return 'Not parsed'


async def parse_files(parse_callbacks):
    await asyncio.gather(*[parse_impl(c) for c in parse_callbacks])
