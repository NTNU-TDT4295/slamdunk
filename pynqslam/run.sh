#!/bin/bash

remote=

if [ -f /etc/slamvis_host ]; then
	remote=$(</etc/slamvis_host)
fi;

dir=$(dirname $BASH_SOURCE[0])

exec $dir/slam $remote
