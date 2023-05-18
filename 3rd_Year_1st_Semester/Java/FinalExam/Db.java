import java.sql.*;

import java.util.*;

public class Db {

   String dbDriver ="com.mysql.cj.jdbc.Driver"; 

   String dbUrl    = "jdbc:mysql://localhost:3306/test?characterEncoding=UTF-8 & serverTimezone=UTC";

   String dbId     = "root";

   String dbPw     = "1234";

   Connection con = null;

   Statement stmt = null;   

   

   Vector<String> v=new Vector<String>();

   

   Db(){

    try{

   Class.forName( dbDriver );

   con = DriverManager.getConnection( dbUrl, dbId, dbPw );  

   stmt = con.createStatement();    

        System.out.println("mysql jdbc test: connect ok!!");

        }catch(Exception e) {

        System.out.println(e);

       } //catch

}//DB_test

   

   

   void inn(String j,String t) {

    try {

 String sql="insert into final(name,number) values('"+j+"','"+t+"')";   

   stmt.executeUpdate(sql);  

     System.out.println("in ok");

     stmt.close();con.close();

    }catch(Exception e) {

     e.printStackTrace();

    }    

   }//inn  

   

   

   

   String get(String t) {

    String price="";

    try {

 String sql="select price from db_book where title='"+t+"'";   

  ResultSet rs=stmt.executeQuery(sql);

  while(rs.next()) {

   price=rs.getString("price");

  }  

   System.out.println("get ok");

   stmt.close(); con.close();

    }catch(Exception e) {

     e.printStackTrace();

    }    

    return price;

   }//get

   

   

   

   Vector sel(String m) {

    v.clear();

     try {

    String sql="select title from db_book where jang= '"+m+"'";

     ResultSet rs=stmt.executeQuery(sql);

       while(rs.next()) {

         String t=rs.getString("title");   

         v.add(t);

         }//while 

      System.out.println("sel ok");

      stmt.close(); con.close();

     }catch(Exception e) {

      e.printStackTrace();

     }    

    return v;

   }//sel

   

   

   

   

   

   

   

   Vector show() {

    try {

     String sql="select * from zipcode";

    ResultSet rs=stmt.executeQuery(sql) ;

    while(rs.next()) {

     String s="";

     String s1=rs.getString("zipcode");

     String s2=rs.getString("sido");

     String s3=rs.getString("gungu");

     String s4=rs.getString("doro");

     s=s1+" "+s2+" "+s3+" "+s4;

      v.add(s);

    }//while

    stmt.close();

    con.close();    

    }catch(Exception e) {

    e.printStackTrace();   

    }    

       return v;

   }//Show   

   

   void del(String d) {

    try {

    String sql="delete from zipcode where doro= '"+d+"'";

    stmt.executeUpdate(sql);

    System.out.println("del ok");

     stmt.close();

     con.close();

    }catch(Exception e) {

        e.printStackTrace();

    }

   }//del   

   

   void up(String s,String p) {

    try {

    String sql="update zipcode set doro='"+p+"' where doro='"+s+"'";

    stmt.executeUpdate(sql);

    System.out.println("up ok");

     stmt.close();

     con.close();

    }catch(Exception e) {

        e.printStackTrace();

    }

   }//up

   

   

   

   

   

   Vector find(String mg) {

    try {

     String sql="select * from zipcode where doro like '%"+mg+"%'";

    ResultSet rs=stmt.executeQuery(sql) ;

    while(rs.next()) {

     String s="";

     String s1=rs.getString("zipcode");

     String s2=rs.getString("sido");

     String s3=rs.getString("gungu");

     String s4=rs.getString("doro");

     s=s1+" "+s2+" "+s3+" "+s4;

      v.add(s);

    }//while   

    stmt.close();

    con.close();

    }catch(Exception e) {

    e.printStackTrace();   

    }    

       return v;

   }//find  

   

   

   public static void main(String args[]) {

     new Db();

   }

}