import os
import ycm_core

# Diese Flags sind für g++ Konfigurationen
flags = [
        '-Wall',
        '-Wextra',
        '-Werror',
        '-std=c++20',
        '-x','c++',
        '-I','/home/giorgio/Bastet_V2/header',
        '-I','/usr/include',
        '-I','/usr/local/include',
        ]
# Diese FUntion findet den Compile-Flags für die aktuelle Conf
def Settings(**kwards):
    return {
            'flags':flags
           }
