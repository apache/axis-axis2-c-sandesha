#!/bin/bash
mysql -u g -p < mysql_schema_svr_create.sh
mysql -u g -p < mysql_schema_client_create.sh
