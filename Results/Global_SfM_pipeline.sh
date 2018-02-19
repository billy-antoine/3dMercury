#!/bin/bash

#Global SfM pipeline

#OpenMVG
#echo "Please enter subject name:"
#read subject

imagePath=$1
MVG_DIR_PATH=$2
MVSPath=$3

MVGPath=$MVG_DIR_PATH/Linux-x86_64-RELEASE
sensorPath=$4/src/openMVG/exif/sensor_width_database/sensor_width_camera_database.txt

cd $imagePath

focal=0
for f in ./images/*
do
	echo "based on $f file..."
	W=`identify $f | cut -f 3 -d " " | sed s/x.*//` #width
	H=`identify $f | cut -f 3 -d " " | sed s/.*x//` #height
	focal=$H
	if (( $W > $H )); then focal=$W; fi;
	break
done
focal=$(($focal * 12))
focal=$(($focal / 10))
echo "Computed focal of: $focal"

mkdir matches
mkdir reconstruction
cd reconstruction
mkdir undistorted_images

echo "Copying images..."
cp -r ../images/* ./undistorted_images

cd $MVGPath

./openMVG_main_SfMInit_ImageListing -i $imagePath/images/ -d $sensorPath -o $imagePath/matches/ -f $focal

./openMVG_main_ComputeFeatures -i $imagePath/matches/sfm_data.json -o $imagePath/matches/

./openMVG_main_ComputeMatches -i $imagePath/matches/sfm_data.json -o $imagePath/matches/

./openMVG_main_IncrementalSfM -i $imagePath/matches/sfm_data.json -m $imagePath/matches/ -o $imagePath/reconstruction/

./openMVG_main_openMVG2openMVS -i $imagePath/reconstruction/sfm_data.bin -o $imagePath/reconstruction/scene.mvs


#OpenMVS

cp $MVSPath/bin/DensifyPointCloud $imagePath/reconstruction
cp $MVSPath/bin/ReconstructMesh $imagePath/reconstruction
cp $MVSPath/bin/RefineMesh $imagePath/reconstruction
cp $MVSPath/bin/TextureMesh $imagePath/reconstruction
cp $MVSPath/bin/Viewer $imagePath/reconstruction

cd $imagePath/reconstruction

./DensifyPointCloud scene.mvs
./ReconstructMesh scene_dense.mvs
./RefineMesh scene_dense_mesh.mvs
./TextureMesh scene_dense_mesh_refine.mvs
./TextureMesh scene_dense_mesh.mvs

rm *.dmap
rm *.log
rm *.svg
rm *.html
rm -r undistorted_images

./Viewer scene_dense_mesh_refine_texture.mvs