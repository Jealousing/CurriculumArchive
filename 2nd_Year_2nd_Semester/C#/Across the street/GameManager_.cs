using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameManager_ : MonoBehaviour
{
    public GameObject OverBG = null;        //게임오버 화면
    public static bool PlayerDie = false;     //플레이어 죽음 여부

    public GameObject StartBG = null;       //게임시작 화면
    public static bool GameStart = false;   //게임시작 여부

    public Text ScoreLavel = null;              //점수판 Text(흰색)
    public Text ScoreLavel1 = null;             //점수판 Text(검은색)
    public Text StaminaLavel = null;           //달리기에 사용하는 스테미나 Text

    public static float Score = 0f;               //점수
    public static float Stamina = 0f;           //스테미나
    public static void ScoreSet(float posz)             //점수설정
    {
        Score = posz * 10f; //z값 *10 = 점수
    }
    public static void StaminaSet(float _Stamina)   //스테미나설정
    {
        Stamina = _Stamina;
    }

    public void _OnClick_Restart()  //재시작 버튼
    {
        OverBG.SetActive(false);    //게임오버 화면을 꺼줌
        PlayerDie = false;              //죽음처리 취소
    }
    public void _OnClick_start()    //게임 시작버튼
    {
        StartBG.SetActive(false);   //게임시작화면 삭제
        GameStart = true;             //게임시작여부
    }
    void Start()
    {
        StartBG.SetActive(true);                            //시작시 게임스타트화면 보여줌   
        OverBG.SetActive(false);                            //게임오버화면은 가림

        ScoreLavel.text = string.Format("");            //처음에 화면에 Text를 안보여주기위해 처리
        ScoreLavel1.text = string.Format("");
        StaminaLavel.text = string.Format("");
    }

    // Update is called once per frame
    void Update()
    {
        if(GameStart)//게임 시작했을 경우
        {   //게임에 필요한 정보들 출력
            ScoreLavel.text = string.Format("Score: {0:0.00}점", GameManager_.Score);    //스코어
            ScoreLavel1.text = string.Format("Score: {0:0.00}점", GameManager_.Score);
            StaminaLavel.text = string.Format("지구력 : {0}", GameManager_.Stamina);       //스테미너
        }
       

        if(PlayerDie)//플레이어가 죽었을 경우
        {
            OverBG.SetActive(true);     //게임오버 화면 띄워줌
            StaminaLavel.text = string.Format("");  //지구력표시는 오버화면에 필요없으니 가려줌
        }
    }
}
