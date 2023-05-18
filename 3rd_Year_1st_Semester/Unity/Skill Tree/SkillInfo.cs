using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class SkillInfo : MonoBehaviour
{

    //조건
    public int Skill_Get_Lv = 0;             //스킬배울 수 있는 레벨
    public int Skill_Get_ParentLv = 1;  //부모스킬 최소레벨

    //스킬정보
    public int Skill_Lv = 0;                    //스킬 현재레벨
    public Text Skill_Lv_Text;
    public int Skill_Max_Lv = 1;            //스킬 최대 레벨
    public bool IsActive_skill = false; //액티브 or 패시브 스킬

    //상태
    public bool Activation = false;                //스킬이 찍혀있는지

    //관련 정보
    private Image image;
    SkillTreeInfo _TreeInfo;
    SkillNode _skillNode;

    public void Save(int num) //저장 
    {
        //bool 정보 저장
        if(Activation)
            PlayerPrefs.SetInt("Activation" + num, 1);
        else
            PlayerPrefs.SetInt("Activation" + num, 0);

        //skill lv 저장
        PlayerPrefs.SetInt("Skill_Lv" + num, Skill_Lv);
    }
    public void Load(int num) // 로드
    {
        //bool 정보를 로드
        int temp =PlayerPrefs.GetInt("Activation" + num);
        if (temp == 1)
        {
            Activation = true;
        }
        else
        {
            Activation = false;
        }
        Skill_Lv=PlayerPrefs.GetInt("Skill_Lv" + num);
        Draw();//그리기
    }
    void Start()
    {
        //정보받아옴
        image = this.GetComponent<Image>();
        _TreeInfo = GameObject.Find("SkillTree").GetComponent<SkillTreeInfo>();
    }

    public void Skill_Info_Reset()
    {
        //스킬 정보 초기화
        Activation = false;
        Skill_Lv = 0;
        Skill_Lv_Text.text = Skill_Lv + "";
    }

    bool CheckParentLv()
    {
        //스킬을 배우기 위한 조건 충족을 위한 부모 스킬 확인
        _skillNode = this.gameObject.GetComponent<SkillNode>();
        if (_skillNode.ParentCheck())//부모노드가 존재하는지 확인
        {
            for (int i = 0; i < _skillNode.Parent_Node_List.Count; i++)
            {
                //부모스킬이 찍혀있으면(조건에맞춰) true
                if (Skill_Get_ParentLv <= _skillNode.Parent_Node_List[i].gameObject.GetComponent<SkillInfo>().Skill_Lv)
                { 
                    return true;
                }
            }
        }
        else//부모가 존재하지않아도 true
        {
            return true;
        }
        return false;//false는 부모가 있지만 안찍혀 있을 경우에만 false
    }

    void SetSkillLv()//스킬레벨 조정
    {
        if (!Activation)//스킬이 안찍혀있어서 비활성화 되어있으면 진행
        {
            if (_TreeInfo.PointDown())//스킬포인트 다운(1)
            {
                Activation = true;//활성화
                Skill_Lv = 1;//처음스킬 활성화 = 1레벨
                Skill_Lv_Text.text = Skill_Lv + ""; // 텍스트 변경
                
            }
        }
        else//이미 활성화 되있으면
        {
            if (Skill_Max_Lv > Skill_Lv) //max레벨 전까지 상승가능
            {
                if (_TreeInfo.PointDown())//스킬포인트다운
                {
                    //스킬레벨 up 및 텍스트 변경
                    Skill_Lv++;
                    Skill_Lv_Text.text = Skill_Lv + "";
                }
            }
        }
    }

    public void ClickImage() //이미지를 클릭하면 발동
    {
        //부모확인
        if(CheckParentLv())
        {
            SetSkillLv();//스킬레벨 조정
        }
    }

    void Draw()//그리기
    {
        //활성화 비활성화에 따른 그리기
        if (Activation)
        {
            Skill_Lv_Text.color = Color.yellow;
            image.color = Color.grey;
            Skill_Lv_Text.text = Skill_Lv + "";
        }
        else
        {
            image.color = Color.white;
            Skill_Lv_Text.color = Color.black;
            Skill_Lv_Text.text = Skill_Lv + "";
        }
    }

    void Update()
    {
        Draw();
    }
}
