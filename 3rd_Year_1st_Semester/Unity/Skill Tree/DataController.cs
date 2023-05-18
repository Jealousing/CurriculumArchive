using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DataController : MonoBehaviour
{
    //데이터 저장 불러오기

    bool GameStart;//시작여부
    public GameObject Panel; //패널
    public void SavePanel()//세이브 패널
    {
        string temp1 = "스킬트리를" + "\n" + "저장하겠습니까?";
        string temp2 = "SkillSave";
        Panel.GetComponent<PanelController>().PanelActive(temp1, temp2);
    }

    public void LoadPanel()//로드 패널
    {
        string temp1 = "스킬트리를" + "\n" + "불러오기하겠습니까?";
        string temp2 = "SkillLoad";
        Panel.GetComponent<PanelController>().PanelActive(temp1, temp2);
    }

    public void Save() //저장
    {
        //Skill Tag가 붙은 objcet 정보 다찾기
        GameObject[] Skill_Objects = GameObject.FindGameObjectsWithTag("Skill");
        for (int i = 0; i < Skill_Objects.Length; i++)//오브젝트 정보저장
        {
            Skill_Objects[i].gameObject.GetComponent<SkillInfo>().Save(i);
        }
        //스킬트리 메인정보 저장
        GameObject.Find("SkillTree").GetComponent<SkillTreeInfo>().Save();
    }

    public void Load() //불러오기
    {
        //저장의 반대
        GameObject[] Skill_Objects = GameObject.FindGameObjectsWithTag("Skill");
        for (int i = 0; i < Skill_Objects.Length; i++)
        {
            Skill_Objects[i].gameObject.GetComponent<SkillInfo>().Load(i);
        }
        GameObject.Find("SkillTree").GetComponent<SkillTreeInfo>().Load();
    }

    private void Start()
    {
        //게임시작 알림
        GameStart = true;
    }

    private void Update()
    {
        if(GameStart)//시작했으면 로드실행
        {
            GameStart = false;
            Load();
        }
    }


}
