# Tulip k-nearest neighborhood plugin

## Description

This plugin builds a k-nearest neighbors graph on a set of nodes. Uses the Euclidian distance.

## Build

Launch one of the CMake project configuration tool and select your build directory. Set the CMAKE_MODULE_PATH variable to the location of the FindTULIP.cmake file (should be &lt;tulip_install_dir&gt;/share/tulip).

More informations [here](http://tulip.labri.fr/TulipDrupal/?q=node/1481).

## Use

The plugin needs the following parameters:

 * _layout_: The LayoutProperty to use to compute distances.
 * _matrix_: The DoubleProperty where the distance will be stored.
 * _k_: Number of neighbors.

## LICENSE

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this program. If not, see [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

