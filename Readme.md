我们知道，make程序能够根据程序中各模块的修改情况，自动判断应对哪些模块重新编译，保证软件是由最新的模块构建的。至于检查哪些模块，以及如何构建软件由makefile文件来决定。

虽然make可以在makefile中进行配置，除此之外我们还可以利用make程序的命令行选项对它进行即时配置。Make命令参数的典型序列如下所示：

make [-f makefile文件名][选项][宏定义][目标]
这里用[]括起来的表示是可选的。命令行选项由破折号“–”指明，后面跟选项，如:

make –e
如果需要多个选项，可以只使用一个破折号，如

make –kr
也可以每个选项使用一个破折号，如

make –k –r
甚至混合使用也行，如

make –e –kr


Make命令本身的命令行选项较多，这里只介绍在开发程序时最为常用的三个，它们是： 
–k： 
如果使用该选项，即使make程序遇到错误也会继续向下运行；如果没有该选项，在遇到第一个错误时make程序马上就会停止，那么后面的错误情况就不得而知了。我们可以利用这个选项来查出所有有编译问题的源文件。

–n： 
该选项使make程序进入非执行模式，也就是说将原来应该执行的命令输出，而不是执行。

–f ： 
指定作为makefile的文件的名称。 如果不用该选项，那么make程序首先在当前目录查找名为makefile的文件，如果没有找到，它就会转而查找名为Makefile的文件。如果您在Linux下使用GNU Make的话，它会首先查找GNUmakefile，之后再搜索makefile和Makefile。按照惯例，许多Linux程序员使用Makefile，因为这样能使Makefile出现在目录中所有以小写字母命名的文件的前面。所以，最好不要使用GNUmakefile这一名称，因为它只适用于make程序的GNU版本。

当我们想构建指定目标的时候，比如要生成某个可执行文件，那么就可以在make命令行中给出该目标的名称；如果命令行中没有给出目标的话，make命令会设法构建makefile中的第一个目标。我们可以利用这一特点，将all作为makefile中的第一个目标，然后将让目标作为all所依赖的目标，这样，当命令行中没有给出目标时，也能确保它会被构建。


除了指明目标和模块之间的依赖关系之外，makefile还要规定相应的规则来描述如何生成目标，或者说使用哪些命令来根据依赖模块产生目标。就上例而言，当make程序发现需要重新构建f1.o的时候，该使用哪些命令来完成呢？很遗憾，到目前为止，虽然make知道哪些文件需要更新，但是却不知道如何进行更新，因为我们还没有告诉它相应的命令。

当然，我们可以使用命令gcc -c f1.c来完成，不过如果我们需要规定一个include目录，或者为将来的调试准备符号信息的话，该怎么办呢？所有这些，都需要在makefile中用相应规则显式地指出。

实际上，makefile是以相关行为基本单位的，相关行用来描述目标、模块及规则（即命令行）三者之间的关系。一个相关行格式通常为：冒号左边是目标（模块）名；冒号右边是目标所依赖的模块名；紧跟着的规则（即命令行）是由依赖模块产生目标所使用的命令。相关行的格式为：

目标：[依赖模块][;命令]
习惯上写成多行形式，如下所示：

目标：[依赖模块]
命令
命令
需要注意的是，如果相关行写成一行，“命令”之前用分号“；”隔开，如果分成多行书写的话，后续的行务必以tab字符为先导。对于makefile而言，空格字符和tab字符是不同的。所有规则所在的行必须以tab键开头，而不是空格键。初学者一定对此保持警惕，因为这是新手最容易疏忽的地方，因为几个空格键跟一个tab键在肉眼是看不出区别的，但make命令却能明察秋毫。


https://www.zybuluo.com/lishuhuakai/note/209302


