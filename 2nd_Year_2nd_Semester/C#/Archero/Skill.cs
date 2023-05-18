using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Skill : MonoBehaviour
{
    //스킬설정

    //버튼
    public Button[] Button;
    //skill목록
    public Sprite[] SkillSprite;
    //이미지모음
    public List<Image> SkillImage = new List<Image>();
    //결과이미지
    public Image Result;

    //결과 모음
    int[] ResultIndex = new int[3];
    //스킬 갯수
    int ItemCnt = 7;


    void Start()
    {
        setting();
    }

    //세팅
    void setting()
    {
        ResultIndex[0] = 0;
        SkillImage[0].sprite = SkillSprite[ResultIndex[0]];
        for (int i =1; i < 3; i++)
        {
            int randomIndex = Random.Range(0, ItemCnt);
            ResultIndex[i]=randomIndex;
            SkillImage[i].sprite = SkillSprite[ResultIndex[i]];
        }
    }

    //다시 활성화되면 세팅
    private void OnEnable()
    {
        setting();
    }

    //index에 따른 스킬 습득
    bool GetSkill(int index)
    {
        if (index == 0)
        {
            PlayerData.PlayerDamage += 50;
            Debug.Log("스킬배움 : attack +50");
            return true;
        }
        if (index == 3)
        {
            if(PlayerTargeting.IsBackway)
            {
                Debug.Log("스킬선택 : back attack 보유중");
                return false;
            }
            else
            {
                PlayerTargeting.IsBackway = true;
                Debug.Log("스킬배움 : back attack");
                return true;
            }
        }
        if (index == 1)
        {

            PlayerData.BounceNum++;
            Debug.Log("스킬배움 : Bounce +1");
            return true;
        }
        if (index == 4)
        {
            if(PlayerTargeting.IsDiagonally)
            {
                Debug.Log("스킬선택 : Diagonally 보유중");
                return false;
            }
            else
            {
                PlayerTargeting.IsDiagonally = true;
                Debug.Log("스킬배움 : Diagonally");
                return true;
            }
        }
        if (index == 5)
        {
            PlayerTargeting.Bulletnum += 1;
            Debug.Log("스킬배움 : Bulletnum +1");
            return true;
        }
        if (index == 2)
        {
            PlayerData.Player_Current_Hp += 200;
            PlayerData.Player_Max_Hp += 200;
            Debug.Log("스킬배움 : Hp +200");
            return true;
        }
        if (index == 6)
        {
            if (PlayerTargeting.IsSideway)
            {
                Debug.Log("스킬선택 : Sideway 보유중");
                return false;
            }
            else
            {
                PlayerTargeting.IsSideway = true;
                Debug.Log("스킬배움 : Sideway");
                return true;
            }
        }

        return false;
    }

    //숨김
    void Hide()
    {
        this.gameObject.SetActive(false);
        Result.gameObject.SetActive(false);
    }

    //클릭!
    public void ClickBtn(int index)
    {
        Result.sprite = SkillSprite[ResultIndex[index]];
        Debug.Log("뭔지:" + ResultIndex[index]);
        if (GetSkill(ResultIndex[index]))
        {
            Result.gameObject.SetActive(true);
            RoomSetting.IsRun=true;
            Invoke("Hide", 1.0f);

        }
    }
}
