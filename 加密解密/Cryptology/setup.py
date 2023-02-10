#!/usr/bin/env python
 
"""
setup.py file for SWIG example
"""
 
from distutils.core import setup, Extension
 
 
Cryptology_module = Extension('_Cryptology',
                           sources=['Cryptology_wrap.cxx','AES.cpp','CrypotologyBase.cpp','SM3.cpp'],
                           )
 
setup (name = 'Cryptology',
       version = '0.1',
       author      = "zjc",
       description = """加密解密cpp""",
       ext_modules = [Cryptology_module],
       py_modules = ["Cryptology"],
       )