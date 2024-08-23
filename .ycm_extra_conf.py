import os
import ycm_core
import subprocess

fin_includepath = subprocess.run(["echo | g++ -xc -xc++ -E -v - 2>&1 | sed -n '/#include <...> search starts here:/,/End of search list./p'"],shell=True,capture_output=True,text=True)
includepaths = fin_includepath.stdout.split('\n')
includeflags = []
for i in range(1,len(includepaths)-1):
    includeflags.append('-isystem')
    includeflags.append(includepaths[i].split(' ')[1])
# Diese Flags sind für g++ Konfigurationen
flags = [
        '-Wall',
        '-Wextra',
        '-Werror',
        '-std=c++20',
        '-x','c++',
        '-I','header'
        ]
flags.extend(includeflags)


# Diese FUntion findet den Compile-Flags für die aktuelle Conf
def Settings(**kwards):
    return {
            'flags':flags
            } 
