using UnityEngine;
using System.Collections;

public class Delay : MonoBehaviour {
	
	public float delayTime = 1.0f;
    private bool isShow = true;
	// Use this for initialization
	void Start () {

	}
	void DelayFunc()
	{
		gameObject.SetActive(true);
	}
    void OnEnable()
    {
        if (isShow)
        {
            gameObject.SetActive(false);
            Invoke("DelayFunc", delayTime); 
            isShow = false;
        }
    }
    void OnDisable()
    {
        isShow = true;
    }
}
