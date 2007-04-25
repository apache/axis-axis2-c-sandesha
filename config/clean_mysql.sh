#!/bin/bash
rm -f logs/*
mysql -u g -p < mysql_schema_svr_clean.sh
mysql -u g -p < mysql_schema_client_clean.sh
rm -f wsf.log
rm -f wsf_svr.log
