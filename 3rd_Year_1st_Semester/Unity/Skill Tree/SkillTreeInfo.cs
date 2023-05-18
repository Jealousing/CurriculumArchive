using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SkillTreeInfo : MonoBehaviour
{
    //스킬포인트
    int Skill_Point_save;
    public int Skill_Point = 1000; //보유
    public Text Skill_Point_Text;

    //유저정보
    public int User_Lv = 100;
    public Text User_Lv_Text;

    public void Save()//저장
    {
        //스킬포인트와 유저레벨 저장
        PlayerPrefs.SetInt("Skill_Point", Skill_Point);
        PlayerPrefs.SetInt("User_Lv", User_Lv);
    }
    public void Load()//로드
    {
        //스킬포인트와 유저레벨 불러오기
        Skill_Point = PlayerPrefs.GetInt("Skill_Point");
        User_Lv = PlayerPrefs.GetInt("User_Lv");
    }

    private void Start()
    {
        //스킬포인트 세이브
        Skill_Point_save = Skill_Point;
    }

    private void FixedUpdate()
    {
        //텍스트 갱신
        Skill_Point_Text.text = "스킬포인트: " + Skill_Point;
        User_Lv_Text.text = "유저레벨: " + User_Lv;
    }

    public bool PointDown()
    {
        //스킬 포인트 감소
        if(Skill_Point>0)
        {
            Skill_Point--;
            return true;
        }
        return false;
    }

    public void SkillReset()
    {
        //리셋
        Skill_Point = Skill_Point_save;
    }

}
