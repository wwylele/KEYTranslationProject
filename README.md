#KEY Translation

This is a Chinese translation project for the game *Kirby's Epic Yarn*.

这里是《毛线卡比》的汉化工程。

###翻译们看这里：

请将翻译文本以UTF-8编码存储到与原文本同名的文件里，上传到`message_bin`文件夹中。
注意翻译文本格式要与原文本一致。格式问题可以参阅`message_bin/example.txt`。

###工具说明

 - `arc`: 用于拆包arc文件的工具，用python2编写。

 - `gfa`: 用于拆包和重组gfa文件的工具，用python3编写。
`gfa_encoder.py`运行前先要编译`bpe.c`为`bpe`，
运行`gfa_encoder.py`时要将工作目录设为`bpe`所在文件夹。

 - `message_bin`: 用于拆包和重组文本文件的工具，用python3编写。

