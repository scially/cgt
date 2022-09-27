# 简介
倾斜常用工具集
1. 支持投影坐标系之间，以及和WGS84之间坐标系互相转换。
2. 支持根据矢量范围线导出对应瓦片。

# 用法说明

##  命令行参数说明
### 基本命令
```sh
Usage: cgt [OPTIONS] SUBCOMMAND

Options:
  -h,--help                   Print this help message and exit
  -v,--version                Display program version information and exit
  -i,--in TEXT:DIR REQUIRED   osg Data path
  -o,--out TEXT:DIR REQUIRED  output location
  --source-srs TEXT           source srs
  --source-origin TEXT        source srs origin (default: 0,0,0)
  -t,--thread UINT            thread

Subcommands:
  transform                   coordinate transform
  export                      export osgb data
  
如果没有设置source-srs,source-origin选项,cgt默认会在输入根目录下寻找metadata.xml
```
### transform
```shell
cgt [options] transform
Options:
  --target-srs TEXT REQUIRED  target srs
  --target-origin TEXT        target srs origin(default: 0,0,0)
```

### export
```shell
cgt [options] export
Options:
  --shapefile TEXT:FILE REQUIRED  export extent
  -c,--copy BOOLEAN               copy tile(default: true)

```
## 示例命令

```sh
# 坐标转换
cgt.exe -i {DATA} -o ${OUT_DIR} transform --target-srs EPSG:4547 --target-origin 435924,2390062,0 
# 导出倾斜
cgt.exe -i {DATA} -o ${OUT_DIR} export --shapefile {shp, geojson}
```

# 数据要求及说明

### 倾斜摄影数据

倾斜摄影数据仅支持 smart3d 格式的 osgb 组织方式：
- 每个瓦片目录下，必须有个和目录名同名的 osgb 文件，否则无法识别根节点；

正确的目录结构示意：

```
- Your-data-folder
  ├ metadata.xml
  └ Data/Tile_000_000/Tile_000_000.osgb
  └ Data2/Tile_000_000/Tile_000_000.osgb
```

# How To Build
1. Windows下建议使用Vcpkg来管理C++依赖，Ubuntu等Linux操作系统可直接使用apt,yum安装。
2. 版本说明：
   1. GCC >= 8.0 OR MSVC 2019以上
   2. GDAL >= 3.0
   3. OSG >= 3.2
3. Visual Studio 选择CMakeLists.txt，导入工程，编译后，会自动将GDAL_DATA和PROJ_LIB拷贝到执行目录下
4. 将OSGPlugins-${Version}文件夹复制到编译目录下