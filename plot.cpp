#include <fstream>
#include <vector>
#include <vtkChart.h>
#include <vtkChartXY.h>
#include <vtkColor.h>
#include <vtkContextView.h>
#include <vtkDoubleArray.h>
#include <vtkLogger.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTable.h>
#include <vtkVector.h>
#include <vtkVectorOperators.h>

#define FILE_NAME_DATA "oscillation.dat"

struct data {
  std::vector<double> t, x, e;
};

void load_data(data& d, const std::string& file_name);
void plot_data(const data& d);

int main(int argc, char **argv) {
  data d;

  load_data(d, FILE_NAME_DATA);
  plot_data(d);

  return 0;
}

void load_data(data& d, const std::string& file_name) {
  std::ifstream file(file_name);

  while(!file.eof()) {
    double t, x, e;
    
    file >> t >> x >> e;
    d.t.push_back(t);
    d.x.push_back(x);
    d.e.push_back(e);
  }
}

void plot_data(const data& d) {
  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkTable> table;

  vtkNew<vtkDoubleArray> arr_t;
  arr_t->SetName("Time (t)");
  table->AddColumn(arr_t);

  vtkNew<vtkDoubleArray> arr_x;
  arr_x->SetName("Displacement (x)");
  table->AddColumn(arr_x);

  vtkNew<vtkDoubleArray> arr_E;
  arr_E->SetName("Energy (E)");
  table->AddColumn(arr_E);

  int n = d.t.size();
  table->SetNumberOfRows(n);
  for (int i = 0; i < n; ++i) {
    table->SetValue(i, 0, d.t[i]);
    table->SetValue(i, 1, d.x[i]);
    table->SetValue(i, 2, d.e[i]);
  }

  vtkNew<vtkContextView> view;
  view->GetRenderWindow()->SetWindowName("LinePlot");
  view->GetRenderer()->SetBackground(colors->GetColor3d("SlateGray").GetData());

  vtkNew<vtkChartXY> chart;
  view->GetScene()->AddItem(chart);
 
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 1);
  line->SetColor(0, 255, 0, 255);
  line->SetWidth(5.0);
  
  line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(table, 0, 2);
  line->SetColor(0, 0, 255, 255);
  line->SetWidth(5.0);

  view->GetRenderWindow()->Render();
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();
}

