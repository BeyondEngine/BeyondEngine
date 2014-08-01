#import "ViewController.h"
#include "stdafx.h"
#include "Framework/Application.h"
#include "Render/RenderManager.h"
#include "Render/Viewport.h"
#include "Render/ios/GLKRenderWindow.h"
#include "Event/CTouchDelegate.h"
#include "Event/GestureState.h"

@interface ViewController () {
    GLKRenderWindow *m_pRenderWindow;
}
@property (strong, nonatomic) EAGLContext *context;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    [self.view setMultipleTouchEnabled:YES];
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    self.preferredFramesPerSecond = 60;
    
    UIPinchGestureRecognizer *pinch = [[UIPinchGestureRecognizer alloc]
                                       initWithTarget:self action:@selector(reportPinch:)];
    [self.view addGestureRecognizer:pinch];
    
    int width = self.view.bounds.size.width;
    int height = self.view.bounds.size.height;
    
    float factor = self.view.contentScaleFactor;
    
    m_pRenderWindow = new GLKRenderWindow(width, height);
    m_pRenderWindow->SetContext(self.context);
    m_pRenderWindow->SetScaleFactor(factor);
    m_pRenderWindow->AddViewport(new CViewport(m_pRenderWindow,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    
    CApplication::GetInstance()->Initialize(width, height);
}

- (void)dealloc
{
    BEATS_SAFE_DELETE(m_pRenderWindow);
    CRenderManager::GetInstance()->SetCurrentRenderTarget(nullptr);
    
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        if(CRenderManager::GetInstance()->GetCurrentRenderTarget() == m_pRenderWindow)
        {
            CRenderManager::GetInstance()->SetCurrentRenderTarget(nullptr);
        }
        self.context = nil;
    }
    
    // Dispose of any resources that can be recreated.
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    int width = self.view.bounds.size.width;
    int height = self.view.bounds.size.height;
    if( width != CApplication::GetInstance()->Width() ||
        height != CApplication::GetInstance()->Height() )
    {
        CApplication::GetInstance()->OnSizeChanged(width, height);
        CRenderManager::GetInstance()->GetCurrentRenderTarget()->SetSize(width, height);
    }
    CApplication::GetInstance()->Update();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    CRenderManager::GetInstance()->SetCurrentRenderTarget(m_pRenderWindow);
    CApplication::GetInstance()->Render();
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    unsigned int tapCount[TOUCH_MAX_NUM] = {0};
    
    int i = 0;
    for(UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x;
        ys[i] = pos.y;
        tapCount[i] = touch.tapCount;
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchBegan(i, (intptr_t*)ids, xs, ys, tapCount);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    unsigned int tapCount[TOUCH_MAX_NUM] = {0};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x;
        ys[i] = pos.y;
        tapCount[i] = touch.tapCount;
        if(touch.tapCount == 1 || touch.tapCount == 2)
        {
            CTouchDelegate::GetInstance()->OnTapped((intptr_t)touch, pos.x, pos.y, touch.tapCount);
        }
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchEnded(i, (intptr_t*)ids, xs, ys, tapCount);
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x;
        ys[i] = pos.y;
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchMoved(i, (intptr_t*)ids, xs, ys);
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* ids[TOUCH_MAX_NUM] = {0};
    float xs[TOUCH_MAX_NUM] = {0.0f};
    float ys[TOUCH_MAX_NUM] = {0.0f};
    unsigned int tapCount[TOUCH_MAX_NUM] = {0};
    
    int i = 0;
    for (UITouch *touch in touches) {
        ids[i] = touch;
        CGPoint pos = [touch locationInView: self.view];
        xs[i] = pos.x;
        ys[i] = pos.y;
        tapCount[i] = touch.tapCount;
        ++i;
        if(i >= TOUCH_MAX_NUM)
            break;
    }
    CTouchDelegate::GetInstance()->OnTouchCancelled(i, (intptr_t*)ids, xs, ys, tapCount);
}

-(void)reportPinch:(UIPinchGestureRecognizer *)recognizer
{
    EGestureState state;
    switch(recognizer.state)
    {
        case UIGestureRecognizerStatePossible:
            state = EGestureState::eGS_POSSIBLE;
            break;
        case UIGestureRecognizerStateBegan:
            state = EGestureState::eGS_BEGAN;
            break;
        case UIGestureRecognizerStateChanged:
            state = EGestureState::eGS_CHANGED;
            break;
        case UIGestureRecognizerStateEnded:
            state = EGestureState::eGS_ENDED;
            break;
        case UIGestureRecognizerStateCancelled:
            state = EGestureState::eGS_CANCELLED;
            break;
        case UIGestureRecognizerStateFailed:
            state = EGestureState::eGS_FAILED;
            break;
    }
    CTouchDelegate::GetInstance()->OnPinched(state, recognizer.scale);
}

@end

