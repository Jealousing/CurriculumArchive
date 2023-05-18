import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;

public class SeoDongJu extends JFrame implements ActionListener,ItemListener 
{ 
	
 JLabel j1,j2,j3,j4,j5,j6,j7; JButton b1,b2; JTextField tf1,tf2,tf3,tf4,tf5,tf6,tf7,tf8,tf9,tf10; 
 JList<String> st; JComboBox<String> jb; JRadioButton bb1,bb2; ButtonGroup g;
 JPanel p9=new JPanel();
 
 SeoDongJu(){
	 
  super("16032048서동주");
  
  j1=new JLabel("자동차 번호판 교체");
  j2=new JLabel("이름: ");
  j3=new JLabel("나이: ");
  j4=new JLabel("원하는 자음: ");
  j5=new JLabel("번호판 색깔");
  j6=new JLabel("앞자리 입력: 10~20사이");
  j7=new JLabel("이름의 번호판 결과");
  
  b1=new JButton("다시작성");  
     b1.addActionListener(this);
  b2=new JButton("입력하기");
  	b2.addActionListener(new Re());

  tf1=new JTextField("",16);
  tf2=new JTextField("",2);
  tf3=new JTextField("",2);
  
  tf4=new JTextField("",2);
  tf5=new JTextField("",2);
  tf6=new JTextField("",2);
  tf7=new JTextField("",2);
  tf8=new JTextField("",2);
  tf9=new JTextField("",2);
  tf10=new JTextField("",2);


  String s[]= {"가","나","다","라"}; 
  st=new JList<String>(s);
  st.addMouseListener(new M2());

  jb=new JComboBox<String>(); 
  jb.addItem("흰색"); jb.addItem("빨강");jb.addItem("파랑");jb.addItem("노랑");
  //jb.addItemListener(this);

  bb1=new JRadioButton("30세 이상",true);
    bb1.addActionListener(new R());

  bb2=new JRadioButton("30세 미만");
    bb2.addActionListener(new R());

  g=new ButtonGroup(); g.add(bb1); g.add(bb2);
  
  //패널설정
  JPanel p1=new JPanel(); p1.setLayout(new FlowLayout());
  p1.add(j1); 
  JPanel p2=new JPanel(); p2.setLayout(new FlowLayout());
  p2.add(j2); p2.add(tf1); 
  JPanel p3=new JPanel(); p3.setLayout(new FlowLayout());
  p3.add(j3); p3.add(bb1); p3.add(bb2); 
  
  JPanel p4=new JPanel(); p4.setLayout(new FlowLayout());
  p4.add(j4); p4.add(st);
  JPanel p5=new JPanel(); p5.setLayout(new FlowLayout());
  p5.add(j5); p5.add(jb); 
  JPanel p6=new JPanel(); p6.setLayout(new FlowLayout());
  p6.add(j6); p6.add(tf2);p6.add(tf3); 
  
  JPanel p7=new JPanel(); p7.setLayout(new FlowLayout());
  p7.add(b1);p7.add(b2); 
  JPanel p8=new JPanel(); p8.setLayout(new FlowLayout());
  p8.add(j7);
  p9.setLayout(new FlowLayout());
  p9.add(tf4);p9.add(tf5);p9.add(tf6);p9.add(tf7);p9.add(tf8);p9.add(tf9);p9.add(tf10);
  
  //패널그룹
  JPanel setp1=new JPanel(); setp1.setLayout(new BorderLayout());
  setp1.add("North",p1); setp1.add("Center",p2);setp1.add("South",p3);
  JPanel setp2=new JPanel(); setp2.setLayout(new BorderLayout());
  setp2.add("North",p4); setp2.add("Center",p5);setp2.add("South",p6);
  JPanel setp3=new JPanel(); setp3.setLayout(new BorderLayout());
  setp3.add("North",p7); setp3.add("Center",p8);setp3.add("South",p9);
  
  this.setLayout(new BorderLayout());
  this.add("North",setp1); this.add("Center",setp2); this.add("South",setp3);

  this.setDefaultCloseOperation(3);  
  this.pack();  
  this.setSize(250,400);
  //this.setBounds(100, 200, 500, 700);

  this.setVisible(true);

 } // 생성자
 class Re implements ActionListener
 {  
  public void actionPerformed(ActionEvent e) 
  {
	  Random r=new Random();
	  int a1;
	  //이름에따른 설정
	  String s1 = tf1.getText()+"이름의 번호판 결과";
	  j7.setText(s1); 
	  
	  s1=tf2.getText()+tf3.getText();
	  int temp = Integer.parseInt(s1);
	  //숫자확인
	  if(9<temp && temp<21)
	  {
		  tf4.setText(tf2.getText());  
		  tf5.setText(tf3.getText());  
		  tf6.setText(st.getSelectedValue());
		  
		  //랜덤으로 숫자결정
		  if(bb1.isSelected())
		  {
			  a1=r.nextInt(4)+1;
			  tf7.setText(Integer.toString(a1));
			  a1=r.nextInt(4)+1;
			  tf8.setText(Integer.toString(a1));
			  a1=r.nextInt(4)+1;
			  tf9.setText(Integer.toString(a1));
			  a1=r.nextInt(4)+1;
			  tf10.setText(Integer.toString(a1));
		  }
		  else
		  {
			  a1=r.nextInt(3)+6;
			  tf7.setText(Integer.toString(a1));
			  a1=r.nextInt(3)+6;
			  tf8.setText(Integer.toString(a1));
			  a1=r.nextInt(3)+6;
			  tf9.setText(Integer.toString(a1));
			  a1=r.nextInt(3)+6;
			  tf10.setText(Integer.toString(a1));
		  }
		  
	  }
	  else
	  {
		  tf4.setText("앞");
		  tf5.setText("자");
		  tf6.setText("리");
		  tf7.setText("잘");
		  tf8.setText("못");
		  tf9.setText("입");
		  tf10.setText("력");
	  }
	 
	  String temp2=tf7.getText()+tf8.getText()+tf9.getText()+tf10.getText();
	 
	  new Db().inn(tf1.toString(),temp2);
	  
	  
	  
	  String rs=(String)jb.getSelectedItem();
	  if(rs.equals("흰색"))
		  p9.setBackground(Color.white);
	  else if(rs.equals("빨강"))
		  p9.setBackground(Color.red);
	  else if(rs.equals("파랑"))
		  p9.setBackground(Color.blue);
	  else if(rs.equals("노랑"))
		  p9.setBackground(Color.yellow);
	  
	  
  }  
 }//Re

 class M2 extends MouseAdapter
 {
  @Override
    public void mouseClicked(MouseEvent e) 
  {
     String rs=(String)st.getSelectedValue();

   }// mouseClicked 
 }//M2

 class R implements ActionListener
 {
  @Override
  public void actionPerformed(ActionEvent e) 
  {  //bb1 bb2
   JRadioButton bb=(JRadioButton)e.getSource(); //가상
     String st=bb.getText();  
  }   
 }//R 

 public static void main(String[] args) 
 {
      new SeoDongJu();
 }//main



@Override
public void actionPerformed(ActionEvent e) {
	// TODO Auto-generated method stub
	
}



@Override
public void itemStateChanged(ItemEvent e) {
	// TODO Auto-generated method stub
	
}

}//class