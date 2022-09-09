# 简介
倾斜坐标转换工具，支持投影坐标系之间，以及和WGS84之间坐标系互相转换。

# 用法说明

##  命令行格式

```sh
Usage: cgt.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--in TEXT:DIR REQUIRED   osg Data path
  -o,--out TEXT:DIR REQUIRED  output location
  --metadata TEXT:FILE        metadata.xml location
  --target-srs TEXT REQUIRED  target srs
  --source-srs TEXT           source srs
  --source-origin TEXT        soure srs origin (default: 0,0,0)
  -t,--thread UINT            thread

如果设置了metadata 选项, source-srs,source-origin选项会被忽略.  
如果没有metadata.xml文件, 可以通过设置source-srs,source-origin来指定倾斜数据的坐标.
```

## 示例命令

```sh
cgt.exe -i ./Data -o ./\DataT --metadata ./Data/metadata.xml --target-srs EPSG:4326 
```

# 数据要求及说明

### 倾斜摄影数据

倾斜摄影数据仅支持 smart3d 格式的 osgb 组织方式：

- 数据目录必须有一个 `Data` 目录的总入口；
- `Data` 目录同级放置一个 `metadata.xml` 文件用来记录模型的位置信息；
- 每个瓦片目录下，必须有个和目录名同名的 osgb 文件，否则无法识别根节点；

正确的目录结构示意：

```
- Your-data-folder
  ├ metadata.xml
  └ Data/Tile_000_000/Tile_000_000.osgb
```


# How To Build
1. Windows下建议使用Vcpkg来管理C++依赖，Ubuntu等Linux操作系统可直接使用apt安装。
2. 版本说明：
   1. GCC >= 8.0 OR MSVC 2019以上
   2. PROJ >= 8.0
   3. OSG >= 3.2
3. VS2019选择CMakeLists.txt，导入工程，编译
4. 将OSGPlugins-${Version}文件夹复制到编译目录下
