## git使用的相关快捷键 ##
1. git checkout -- test.cpp //还原该文件到上次提交
2. git reset HEAD test.cpp //将该文件从暂存区中取
3. git rm test.cpp //删除文件
4. git mv test.cpp .\home\test.cpp //重命名文件，或移动文件
5. git reset --hard HEAD^ //整个项目回滚版本，回到上n个commit的版本(HEAD后面的^可以有n个)
6. git reset --hard 8e1b577da4353e618ed358f46ed5ba567f8b04b9//整个项目回退版本到输入log中对应版本的编号，可以只输入前7位
7. git checkout 8e1b577da4353e618ed358f46ed5ba567f8b04b9 -- version.cpp//只选择version.cpp文件回退到该log对应版本