@echo on
..\..\..\..\tools\dagor3_cdk\util64\dsc2-hlsl11-dev.exe .\shaders_dx11.blk -q -shaderOn -nodisassembly -commentPP -codeDumpErr -bones_start 70  -o ..\..\..\..\_output\shaders\skiesSample-game~dx11 %1 %2 %3
@echo off
