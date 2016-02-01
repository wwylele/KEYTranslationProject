#KEY Translation

This is a Chinese translation project for the game *Kirby's Epic Yarn*.

这里是《毛线卡比》的汉化工程。

###翻译们看这里：

请将翻译文本以UTF-8编码存储到与原文本同名的文件里，上传到`message_tran`文件夹中。
注意翻译文本格式要与原文本一致。格式问题可以参阅`message_tran/example.txt`。

游戏原版文本（美版&日版）请到这里下载: http://pan.baidu.com/s/1eQL62lG 密码: qxa4

###工具说明

工具运行环境：Windows 32/64

*工具尚不支持Linux等其它系统，因为工具`brfnt/brfnt_mod`中使用了Windows API。欢迎改进。

工具运行前请先准备:

- gcc (或其它C/C++编译器，但需要手动编译源代码)
- python 3

Clone此Repo，在Repo根目录下创建文件夹`raw`。

从游戏ROM提取的子文件，将它们放置到`./raw/`文件夹下。所需文件有：

- `/message/EnglishUS/` 目录下的所有文件
- `/env/Common/GameFont1.gfa`
- `/env/Common/GameFont2.gfa`

字体设置：

`./brfnt/brfnt_mod/main.cpp` 的开头两行记录有汉化所用的两个字体。请安装对应字体，或将该文件中的字体名改为已安装的字体。

工具使用方法：

第一次使用前先运行`prepare.py`，以后就不用再运行这个了。但如果下一步出现问题，请尝试再运行一次`prepare.py`。

每次构建时运行`build_all_message.py`，构建出来的游戏子文件会出现在`./build/`文件夹下，然后将这些文件替换掉ROM里的文件即可
