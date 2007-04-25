#!/bin/bash
mysql -u g -p < mysql_schema_svr_clean.sh
mysql -u g -p < mysql_schema_client_clean.sh
