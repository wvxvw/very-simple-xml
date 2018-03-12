#!/usr/bin/env python3.6
# -*- coding: utf-8 -*-

from setuptools import setup, Extension
from Cython.Distutils import build_ext


setup(
    install_requires=['cython'],
    packages=['very_simple_xml'],
    zip_safe=False,
    name='very_simple_xml',
    version='0.0.1',
    description='Very simplified async SAX XML parser',
    author='Oleg Sivokon',
    author_email='olegs@traiana.com',
    url='TBD',
    license='PROPRIETARY',
    cmdclass={"build_ext": build_ext},
    scripts=['bin/very-simple-xml'],
    ext_modules=[
        Extension(
            'very_simple_xml.wrapped',
            [
                'very_simple_xml/lib/bindings.c',
                'very_simple_xml/lib/pool.c',
                'very_simple_xml/lib/simple_xml.lex.c',
                'very_simple_xml/lib/simple_xml.tab.c',
                'very_simple_xml/wrapped.pyx'],
            libraries=[],
            include_dirs=[]
        )
    ]
)
