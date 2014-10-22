#!/bin/bash
#
# Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        migrate.sh
# @author      Pawel Wieczorek <p.wieczorek2@samsung.com>
# @brief
#

CYNARA_USER="cynara"
CYNARA_GROUP="cynara"

STATE_PATH="/var/cynara"

# Create Cynara's database directory:
mkdir -p ${STATE_PATH}/db

# Create contents of minimal database: first index file, then default bucket
echo ';0x0;' > ${STATE_PATH}/db/buckets
touch ${STATE_PATH}/db/_

# Set proper permissions for newly created database
chown -R ${CYNARA_USER}:${CYNARA_GROUP} ${STATE_PATH}/db

# Set proper SMACK labels for newly created database
chsmack -a "System" ${STATE_PATH}/db
# Uncomment following line if necessary
# (default labels: User[by this script], _[by spec %files section])
#chsmack -a "System" ${STATE_PATH}/db/*
