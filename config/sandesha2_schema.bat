sqlite3 sandesha2 "drop table create_seq;"
sqlite3 sandesha2 "drop table  invoker;"
sqlite3 sandesha2 "drop table  sender"
sqlite3 sandesha2 "drop table  next_msg"
sqlite3 sandesha2 "drop table  seq_property"
sqlite3 sandesha2 "drop table  msg"

sqlite3 sandesha2 "create table create_seq(create_seq_msg_id varchar(100) primary key, internal_seq_id varchar(200), seq_id varchar(200), create_seq_msg_store_key varchar(100), ref_msg_store_key varchar(100))"
sqlite3 sandesha2 "create table invoker(msg_ctx_ref_key varchar(100) primary key, msg_no long, seq_id varchar(200), is_invoked boolean)"
sqlite3 sandesha2 "create table sender(msg_id varchar(100) primary key, msg_ctx_ref_key varchar(100), internal_seq_id varchar(200), sent_count int, msg_no long, send boolean, resend boolean, time_to_send long, msg_type int, seq_id varchar(200), wsrm_anon_uri varchar(100), to_address varchar(100))"
sqlite3 sandesha2 "create table next_msg(seq_id varchar(200) primary key, ref_msg_key varchar(100), polling_mode boolean, msg_no long)" 
sqlite3 sandesha2 "create table seq_property(id varchar(200) primary key, seq_id varchar(200), name varchar(200), value varchar(200))"
sqlite3 sandesha2 "create table msg(stored_key varchar(200) primary key, msg_id varchar(200), soap_env_str text, soap_version int, transport_out varchar(100), op varchar(100), svc varchar(100), svc_grp varchar(100), op_mep varchar(100), to_url varchar(200), reply_to varchar(200), transport_to varchar(200), execution_chain_str varchar(200), flow int, msg_recv_str varchar(200), svr_side boolean, in_msg_store_key varchar(200), prop_str varchar(8192), action varchar(200))"




