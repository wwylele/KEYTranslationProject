#KEY Translation

This is a Chinese translation project for the game *Kirby's Epic Yarn*.

这里是《毛线卡比》的汉化工程。

###翻译们看这里：

请将翻译文本以UTF-8编码存储到与原文本同名的文件里，上传到`message_tran`文件夹中。
注意翻译文本格式要与原文本一致。格式问题可以参阅`message_tran/example.txt`。

###工具说明

工具运行环境：Windows 32/64

工具运行前请先准备:

- gcc (或其它C/C++编译器，但需要手动编译源代码)
- python 3
- 从游戏ROM提取的子文件，将它们放置到`./raw/`文件夹下。所需文件有：
- `/message/EnglishUS/` 目录下的所有文件
- `/env/Common/GameFont1.gfa`
- `/env/Common/GameFont2.gfa`

工具使用方法：

第一次使用前先运行`prepare.py`，以后就不用再运行这个了。

每次构建时运行`build_all_message.py`，构建出来的游戏子文件会出现在`./build/`文件夹下，然后将这些文件替换掉ROM里的文件即可
