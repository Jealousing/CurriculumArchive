using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class SkillNode : MonoBehaviour
{
    //부모노드 자식노드
    public List<GameObject> Parent_Node_List;
    public List<GameObject> Child_Node_List;

    //라인랜더러(노드끼리 이어주는선)
    LineRenderer _lines=new LineRenderer();
    int line_count=0 ;

    public bool ParentCheck()
    {
        //부모스킬이 있는지 확인
        for(int i =0;i<Parent_Node_List.Count;i++)
        {
            if(Parent_Node_List[i].gameObject!=null)
            {
                return true;//있으면 true
            }
        }
        return false;//없음
    }


    void Child_LineRender()
    {
        //자식노드와 이어주는 라인랜더러 생성
        int minus_num = 1;
        //라인렌더러 정보받아옴
        _lines = GetComponent<LineRenderer>();
        _lines.material.color = Color.black;//기본색설정
        //굵기 설정
        _lines.startWidth = 5f;
        _lines.endWidth = 5f;
        line_count = 0;//라인 수
        
        //자식노드 확인해서 라인의 갯수를 정함
        for (int i=0;i<Child_Node_List.Count;i++)
        {
            if(Child_Node_List[i].gameObject!=null)
            {
                line_count++;
            }
        }
        //라인의 갯수 설정
        if (line_count != 0)
        {
            _lines.positionCount = line_count * 2;
        }
        
        //라인 움직임 설정
        for ( int i=0;i<line_count*2;i++)
        {
            if(i%2==0)
            {
                _lines.SetPosition(i, new Vector3(this.transform.position.x, this.transform.position.y - 5, this.transform.position.z));
            }
            else
            {
                _lines.SetPosition(i, new Vector3(Child_Node_List[i - minus_num].transform.position.x, Child_Node_List[i - minus_num].transform.position.y - 5, Child_Node_List[i - minus_num].transform.position.z));
                minus_num++;
            }
        }


    }

    void Start()
    {
        Child_LineRender(); //시작시 라인그림
    }

    private void Update()
    {
        Child_LineRender();
    }

}
