# Image Segmentation with Min-Cut/Max-Flow

## Structure :
- `include/` : Headers
- `src/` : Source files
- `data/` : Input and Output Images

## Compilation :
- `make` or `make all` to compile the project
- `make release` to compile the project without sanitizers
- `make clean` to clean the project

## Use :
```bash 
./bin/seg input_image_path mask_path [contrast_factor]
```
> **Note :** `contrast_factor` (or *sigma*) is optional.

## TODO : Flexible user choice for init pixels