CREATE SEQUENCE seq_mck_table_id;

CREATE TABLE mck_table (
    mck_id			  NUMBER(10),
    mck_default			  CHAR,
    mck_athena_version            VARCHAR2(20),
    mck_creator          	  VARCHAR2(100),
    mck_creation_date    	  TIMESTAMP,
    mck_comment          	  VARCHAR2(1000),
    CONSTRAINT        	   	  mck_pk		       PRIMARY KEY(mck_id),
    CONSTRAINT           	  mck_default_nn	       CHECK(mck_default IN (0,1)),
    CONSTRAINT           	  mck_athena_version_nn        CHECK(mck_athena_version IS NOT NULL),
    CONSTRAINT           	  mck_creator_nn               CHECK(mck_creator IS NOT NULL)
);

CREATE SEQUENCE seq_smck_table_id;

CREATE TABLE smck_table (
    smck_id			  NUMBER(10),
    smck_slice_type      	  VARCHAR2(50),
    smck_tool_type      	  VARCHAR2(50),
    smck_tool_patch_version   	  VARCHAR2(50),
    smck_processing_step 	  VARCHAR2(10),
    smck_config          	  CLOB,
    smck_config_hash          	  VARCHAR2(128),
    smck_default         	  CHAR,
    smck_athena_version  	  VARCHAR2(20),
    smck_creator         	  VARCHAR2(100),
    smck_creation_date   	  TIMESTAMP,
    smck_comment         	  VARCHAR2(1000),
    CONSTRAINT           	  smck_pk		       PRIMARY KEY(smck_id),
    CONSTRAINT           	  smck_slice_type_nn           CHECK(smck_slice_type IS NOT NULL),
    CONSTRAINT           	  smck_tool_type_nn            CHECK(smck_tool_type IS NOT NULL),
    CONSTRAINT           	  smck_tool_patch_version_u    UNIQUE(smck_tool_patch_version),
    CONSTRAINT           	  smck_tool_patch_version_nn   CHECK(smck_tool_patch_version IS NOT NULL),
    CONSTRAINT           	  smck_processing_step_nn      CHECK(smck_processing_step IS NOT NULL),
    CONSTRAINT           	  smck_config_nn               CHECK(smck_config IS NOT NULL),
    CONSTRAINT           	  smck_config_hash_u           UNIQUE(smck_tool_type,smck_processing_step,smck_config_hash),
    CONSTRAINT           	  smck_config_hash_nn          CHECK(smck_config_hash IS NOT NULL),
    CONSTRAINT           	  smck_default_nn              CHECK(smck_default IN (0,1)),
    CONSTRAINT           	  smck_athena_version_nn       CHECK(smck_athena_version IS NOT NULL),
    CONSTRAINT           	  smck_creator_nn              CHECK(smck_creator IS NOT NULL)
);

CREATE TABLE mck_to_smck_link (
    link_mck                      NUMBER(10),
    link_smck            	  NUMBER(10),
    CONSTRAINT           	  link_u		       UNIQUE(link_mck,link_smck),
    CONSTRAINT           	  link_mck_nn                  CHECK(link_mck IS NOT NULL),
    CONSTRAINT           	  link_mck_fk                  FOREIGN KEY(link_mck) REFERENCES mck_table(mck_id),
    CONSTRAINT           	  link_smck_nn                 CHECK(link_smck IS NOT NULL),
    CONSTRAINT       	 	  link_smck_fk                 FOREIGN KEY(link_smck) REFERENCES smck_table(smck_id)
);

CREATE TABLE mck_to_smk_link (
    smk_link_mck                  NUMBER(10),
    smk_link_smk            	  NUMBER(10),
    smk_link_creator          	  VARCHAR2(100),
    smk_link_creation_date    	  TIMESTAMP,
    smk_link_comment          	  VARCHAR2(1000),
    CONSTRAINT           	  smk_link_mck_nn              CHECK(smk_link_mck IS NOT NULL),
    CONSTRAINT           	  smk_link_mck_fk              FOREIGN KEY(smk_link_mck) REFERENCES mck_table(mck_id),
    CONSTRAINT           	  smk_link_smk_u	       UNIQUE(smk_link_smk),
    CONSTRAINT           	  smk_link_smk_nn              CHECK(smk_link_smk IS NOT NULL),
    CONSTRAINT           	  smk_link_creator_nn          CHECK(smk_link_creator IS NOT NULL)
);

CREATE INDEX mck_default_index ON mck_table(mck_default);
CREATE INDEX link_mck_index    ON mck_to_smck_link(link_mck);
CREATE INDEX link_smck_index   ON mck_to_smck_link(link_smck);
