#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <vtkAnimationScene.h>
#include <vtkCamera.h>
#include <vtkChart.h>
#include <vtkChartXY.h>
#include <vtkColor.h>
#include <vtkContextView.h>
#include <vtkCylinderSource.h>
#include <vtkLogger.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>
#include <vtkdoubleArray.h>

#define FILE_NAME_DATA "oscillation.dat"

struct data {
  std::vector<double> t, x, e;
};

void load_data(data &d, const std::string &file_name);
void animate_data(const data &d, const bool real);

int main(int argc, char **argv) {
  data d;

  load_data(d, FILE_NAME_DATA);
  animate_data(d, argc >= 2 && std::string(argv[1]) == "-real");

  return 0;
}

void load_data(data &d, const std::string &file_name) {
  std::ifstream file(file_name);

  while (!file.eof()) {
    double t, x, e;

    file >> t >> x >> e;
    d.t.push_back(t);
    d.x.push_back(x);
    d.e.push_back(e);
  }
}

class ActorAnimator {
public:
  ActorAnimator(vtkActor *Actor, const vtkVector3d &StartPosition,
                const vtkVector3d &EndPosition);

  ActorAnimator();

  ~ActorAnimator() = default;

  void SetActor(vtkActor *actor);

  void SetSource(vtkCylinderSource *source);

  void SetData(const data *d);

  void AddObserversToCue(vtkAnimationCue *cue);

private:
  void Start();

  void Tick(vtkObject *vtkNotUsed(caller), unsigned long vtkNotUsed(event),
            void *calldata);

  void End();

  vtkActor *Actor;
  vtkCylinderSource *Source;
  const data *d;
  vtkVector3d StartPosition;
  vtkVector3d EndPosition;
};

ActorAnimator::ActorAnimator(vtkActor *Actor, const vtkVector3d &StartPosition,
                             const vtkVector3d &EndPosition)
    : Actor(Actor), StartPosition(StartPosition), EndPosition(EndPosition) {}

ActorAnimator::ActorAnimator()
    : Actor(nullptr), StartPosition(0, 0, 0), EndPosition(0.5, 0.5, 0.5) {}

void ActorAnimator::SetActor(vtkActor *actor) { this->Actor = actor; }

void ActorAnimator::SetSource(vtkCylinderSource *source) {
  this->Source = source;
}

void ActorAnimator::SetData(const data *d) { this->d = d; }

void ActorAnimator::AddObserversToCue(vtkAnimationCue *cue) {
  cue->AddObserver(vtkCommand::StartAnimationCueEvent, this,
                   &ActorAnimator::Start);
  cue->AddObserver(vtkCommand::EndAnimationCueEvent, this, &ActorAnimator::End);
  cue->AddObserver(vtkCommand::AnimationCueTickEvent, this,
                   &ActorAnimator::Tick);
}

void ActorAnimator::Start() {
  vtkLogF(INFO, "Start!");
}

void ActorAnimator::Tick(vtkObject *vtkNotUsed(caller),
                         unsigned long vtkNotUsed(event), void *calldata) {
  vtkAnimationCue::AnimationCueInfo *info =
      reinterpret_cast<vtkAnimationCue::AnimationCueInfo *>(calldata);
  const double t = (info->AnimationTime - info->StartTime);
  const double height = (400 / *std::max_element(d->x.begin(), d->x.end())) * d->x[t];

  // this->Actor->SetPosition(this->Actor->GetPosition() + vtkVector3d{height, 0, 0});
  this->Source->SetHeight(height);
  this->Source->Update();

  const std::string log(std::to_string(height));
  vtkLogF(INFO, log.c_str());
}

void ActorAnimator::End() {
  vtkLogF(INFO, "End!");
}

void animate_data(const data &d, const bool real) {
  int n = d.t.size();
  double t0 = d.t[0];
  double tf = d.t[n - 1];
  double time_step = tf / n;

  vtkLogger::Init();

  vtkNew<vtkNamedColors> colors;
  vtkColor3d cylinderColor = colors->GetColor3d("blue");
  vtkColor3d backgroundColor = colors->GetColor3d("Peacock");

  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;

  ren1->SetBackground(backgroundColor.GetData());
  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(ren1);

  vtkNew<vtkCylinderSource> cylinderSource;
  cylinderSource->SetResolution(31);
  cylinderSource->SetRadius(100);
  cylinderSource->SetHeight((400 / *std::max_element(d.x.begin(), d.x.end())) * d.x[0]);

  vtkNew<vtkPolyDataMapper> cylinderMapper;
  cylinderMapper->SetInputConnection(cylinderSource->GetOutputPort());

  vtkNew<vtkActor> cylinder;
  cylinder->SetMapper(cylinderMapper);
  cylinder->SetOrientation(0, 0, -90);
  cylinder->GetProperty()->SetDiffuseColor(cylinderColor.GetData());

  ren1->AddActor(cylinder);

  vtkNew<vtkAnimationScene> scene;
  if (real) {
    vtkLogF(INFO, "real-time mode");
    scene->SetModeToRealTime();
  } else {
    vtkLogF(INFO, "sequence mode");
    scene->SetModeToSequence();
  }
  scene->SetLoop(1);
  scene->SetFrameRate(time_step);
  scene->SetStartTime(t0);
  scene->SetEndTime(tf);
  scene->AddObserver(vtkCommand::AnimationCueTickEvent, renWin.GetPointer(),
                     &vtkWindow::Render);

  vtkNew<vtkAnimationCue> cue2;
  cue2->SetStartTime(t0);
  cue2->SetEndTime(tf);
  scene->AddCue(cue2);

  ActorAnimator animateCylinder;
  animateCylinder.SetActor(cylinder);
  animateCylinder.SetSource(cylinderSource);
  animateCylinder.SetData(&d);
  animateCylinder.AddObserversToCue(cue2);

  renWin->SetWindowName("AnimateActors");

  renWin->Render();
  ren1->ResetCamera();
  ren1->GetActiveCamera()->Dolly(.5);
  ren1->ResetCameraClippingRange();

  iren->Start(); // TODO: move this below play & stop, when a fix is available

  scene->Play();
  scene->Stop();
}
