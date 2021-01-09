# grep

This is custom 'grep' command.

## How to use
'''zsh
$ cat hoge
1    hogehoge
2    foofoo
3    zshzsh

$grep hoge hoge
hogehoge
'''

You can use option.

'''zsh
$ grep -n hoge hoge
1   hogehoge
'''

'''zsh
$ grep -v hoge hoge
foofoo
zshzsh
'''
