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
	 
  super("16032048������");
  
  j1=new JLabel("�ڵ��� ��ȣ�� ��ü");
  j2=new JLabel("�̸�: ");
  j3=new JLabel("����: ");
  j4=new JLabel("���ϴ� ����: ");
  j5=new JLabel("��ȣ�� ����");
  j6=new JLabel("���ڸ� �Է�: 10~20����");
  j7=new JLabel("�̸��� ��ȣ�� ���");
  
  b1=new JButton("�ٽ��ۼ�");  
     b1.addActionListener(this);
  b2=new JButton("�Է��ϱ�");
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


  String s[]= {"��","��","��","��"}; 
  st=new JList<String>(s);
  st.addMouseListener(new M2());

  jb=new JComboBox<String>(); 
  jb.addItem("���"); jb.addItem("����");jb.addItem("�Ķ�");jb.addItem("���");
  //jb.addItemListener(this);

  bb1=new JRadioButton("30�� �̻�",true);
    bb1.addActionListener(new R());

  bb2=new JRadioButton("30�� �̸�");
    bb2.addActionListener(new R());

  g=new ButtonGroup(); g.add(bb1); g.add(bb2);
  
  //�гμ���
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
  
  //�гα׷�
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

 } // ������
 class Re implements ActionListener
 {  
  public void actionPerformed(ActionEvent e) 
  {
	  Random r=new Random();
	  int a1;
	  //�̸������� ����
	  String s1 = tf1.getText()+"�̸��� ��ȣ�� ���";
	  j7.setText(s1); 
	  
	  s1=tf2.getText()+tf3.getText();
	  int temp = Integer.parseInt(s1);
	  //����Ȯ��
	  if(9<temp && temp<21)
	  {
		  tf4.setText(tf2.getText());  
		  tf5.setText(tf3.getText());  
		  tf6.setText(st.getSelectedValue());
		  
		  //�������� ���ڰ���
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
		  tf4.setText("��");
		  tf5.setText("��");
		  tf6.setText("��");
		  tf7.setText("��");
		  tf8.setText("��");
		  tf9.setText("��");
		  tf10.setText("��");
	  }
	 
	  String temp2=tf7.getText()+tf8.getText()+tf9.getText()+tf10.getText();
	 
	  new Db().inn(tf1.toString(),temp2);
	  
	  
	  
	  String rs=(String)jb.getSelectedItem();
	  if(rs.equals("���"))
		  p9.setBackground(Color.white);
	  else if(rs.equals("����"))
		  p9.setBackground(Color.red);
	  else if(rs.equals("�Ķ�"))
		  p9.setBackground(Color.blue);
	  else if(rs.equals("���"))
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
   JRadioButton bb=(JRadioButton)e.getSource(); //����
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