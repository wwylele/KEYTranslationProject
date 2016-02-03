#KEY Translation

This is a Chinese translation project for the game *Kirby's Epic Yarn*.

这里是《毛线卡比》的汉化工程。

###翻译们看这里：

请将翻译文本以UTF-8编码存储到与原文本同名的文件里，上传到`message_tran`文件夹中。
注意翻译文本格式要与原文本一致。格式问题可以参阅[`message_tran/example.txt`](https://github.com/wwylele/KEYTranslationProject/blob/master/message_tran/example.txt)。

游戏原版文本（美版&日版）请到这里下载: http://pan.baidu.com/s/1eQL62lG 密码: qxa4

###工具说明

工具运行环境：Windows 32/64

*工具尚不支持Linux等其它系统，因为工具`brfnt/brfnt_mod`中使用了Windows API。欢迎改进。

工具运行前请先准备:

- gcc 或 msvc（或其它C/C++编译器，但需要手动编译源代码）
- python 3
- 《毛线卡比》美版或日版ROM（日版ROM未测试过，只是理论上可以）
- Wii游戏ROM文件提取及导入工具（如WiiScrubber）

注意设置好相关的环境变量。在PATH中添加python和gcc所在文件夹（`python.exe` 和 `gcc.exe`所在文件夹）。
如果使用msvc的话，需要在运行`prepare.py`之前先运行msvc目录下的`vcvarsall.bat`设置好所有相关变量
（可能的路径为`C:\Program Files\Microsoft Visual Studio 14.0\VC\vcvarsall.bat`）。

Clone此Repo，在Repo根目录下创建文件夹`raw`。

从游戏ROM提取的子文件，将它们放置到`./raw/`文件夹下。所需文件有：

- `/message/EnglishUS/` 目录下的所有文件（如果是日版游戏，则使用目录`/message/Japanese/`）
- `/env/Common/GameFont1.gfa`
- `/env/Common/GameFont2.gfa`

字体设置：

`./brfnt/brfnt_mod/main.cpp` 的开头两行记录有汉化所用的两个字体。请安装对应字体，或将该文件中的字体名改为已安装的字体。

工具使用方法：

第一次使用前先运行`prepare.py`，以后就不用再运行这个了。但如果下一步出现问题，请尝试再运行一次`prepare.py`。

运行`extract_all_message.py`可提取所有游戏原文本，文本会生成到`./build/message_extract/`文件夹下

运行`build_all_message.py`可根据修改后的文本（存储在`./message_tran/`文件夹下）重构ROM的子文件，
重构的子文件会生成到`./build/`文件夹下，
将重构的子文件导入到游戏的原来位置即可完成游戏文本的修改。
（请无视`cjkset`,`GameFont1.brfnt`,`GameFont2.brfnt`三个文件）
`/message/*/`下的文件可以选择性地导入，每次导入时只导入`message_tran`里修改过的对应文件即可。
（但`GameFont1.gfa`和`GameFont2.gfa`必须每次修改都导入一遍）
