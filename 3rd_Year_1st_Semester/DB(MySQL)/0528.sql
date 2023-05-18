USE cookDB;
CREATE OR REPLACE VIEW v_userTBL 
AS 
      SELECT userID, userName, addr FROM userTBL;

SELECT * FROM v_userTBL;
UPDATE v_userTBL SET addr = '서울' WHERE userID='LKK';
insert INTO v_userTBL (userID,userName,addr) VALUES ( 'KBM','김병만','서울');
SELECT * FROM INFORMATION_SCHEMA.VIEWS 
    WHERE TABLE_SCHEMA = 'cookDB' ; -- AND TABLE_NAME = 'v_sum';
