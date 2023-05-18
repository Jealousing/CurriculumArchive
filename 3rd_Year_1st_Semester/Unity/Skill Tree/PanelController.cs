using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PanelController : MonoBehaviour
{
    //패널 생성 스크립트
    
    //스킬트리 데이터
    public GameObject SkillTree;
    //질문
    public Text Question;
    //string tmep
    string temp;

    //패널생성
    public void PanelActive(string _text,string _temp)
    {
        Question.text = _text;
        temp = _temp;
        this.gameObject.SetActive(true);
    }
    //yes 버튼 (실행)
    public void YesButton()
    {
        this.gameObject.SetActive(false);
        if(temp=="SkillReset")
        {
            SkillTree.GetComponent<SkillReset>().SkillPointReset();
        }
        else if(temp=="SkillSave")
        {
            SkillTree.GetComponent<DataController>().Save();
        }
        else if (temp == "SkillLoad")
        {
            SkillTree.GetComponent<DataController>().Load();
        }
        else if(temp== "SkillExit")
        {
            SkillTree.GetComponent<Exit>().ExitBye();
        }

    }
    //no 버튼 (취소)
    public void NoButton()
    {
        this.gameObject.SetActive(false);
    }

}
