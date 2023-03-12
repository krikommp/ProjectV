# ProjectV-UE

UE项目demo

## 设计注意事项

1. 模型设计  
    以100为单位进行设计

## UE代码相关
1. 编辑器模块声明放在 xxxEditor.Target.cs 文件中

## 打包

1. puerts 插件  
   需要在 Edit / Project Settings / Packaging / Additional Non-Asset Directories To Packages 下加入 Content/JavaScripts 目录  
    
2. 打包错误记录
   1. `can not find Binded JavaScript Object`  
      这个问题会在当C++父类override了 `OnConstruction` 函数，并且实现一些逻辑  
      到TS子类中一旦extends这个C++父类，那么在打包的时候会报 `can not find Binded JavaScript Object`   
      这将会导致一些函数无法正常调用，致命性错误  
      > 解决方式：
      > 1. 需要看源码修改，目前不清楚是什么原因导致，怀疑是因为ts无法运行在Editor环境
      > 2. 所有override了OnConstruction函数的类都不要用ts去去extends它