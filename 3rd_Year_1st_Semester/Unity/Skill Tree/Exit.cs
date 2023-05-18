using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Exit : MonoBehaviour
{
    public GameObject Panel;
    //종료관련
    public void ExitBye()
    {
        Application.Quit();
    }

    public void ExitPanel()//종료패널 생성
    {
        string temp1 = "스킬트리를" + "\n" + "종료하겠습니까?";
        string temp2 = "SkillExit";
        Panel.GetComponent<PanelController>().PanelActive(temp1, temp2);
    }

    private void Update()
    {
        //esc = 종료
        if(Input.GetKeyDown(KeyCode.Escape))
        {
            ExitPanel();
        }
    }
}
