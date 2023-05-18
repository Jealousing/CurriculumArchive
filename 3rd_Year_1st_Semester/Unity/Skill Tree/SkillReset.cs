using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkillReset : MonoBehaviour
{
    public GameObject Panel;
    //스킬트리 리셋
    public void SkillPanel()//스킬패널 생성
    {
        string temp1 = "스킬트리를"+"\n"+"초기화하겠습니까?";
        string temp2 = "SkillReset";
        Panel.GetComponent<PanelController>().PanelActive(temp1, temp2);
    }
    public void SkillPointReset()//리셋
    {
        //스킬트리 object찾아서 리셋시킴
        GameObject.Find("SkillTree").GetComponent<SkillTreeInfo>().SkillReset();
        //Skill TAG붙은 애들 정보 받아옴
        GameObject[] Skill_Objects = GameObject.FindGameObjectsWithTag("Skill");
        for (int i = 0; i < Skill_Objects.Length; i++)
        {
            //초기화
            Skill_Objects[i].GetComponent<SkillInfo>().Skill_Info_Reset();
        }
    }
}
