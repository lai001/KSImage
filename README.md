# KSImage
 
Windows 10  
Visual Studio 2017  
c++ 17  
[xmake](https://github.com/xmake-io/xmake)


## build
```
[Powershell]

git clone https://github.com/lai001/KSImage.git 
cd KSImage/Example
xmake config -m debug -a x64
xmake project -k vsxmake -a x64 -m debug
```