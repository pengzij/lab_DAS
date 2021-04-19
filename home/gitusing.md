## git使用的相关快捷键 ##
1. git checkout -- test.cpp //还原该文件到上次提交
2. git reset HEAD test.cpp //将该文件从暂存区中取
3. git rm test.cpp //删除文件
4. git mv test.cpp .\home\test.cpp //重命名文件，或移动文件
5. git reset --hard HEAD^ //回滚版本，回到上n个commit的版本(HEAD后面的^可以有n个)