/*
 * Developed by Rodrigo Amaducci (rodrigo.amaducci@uam.es) from the Plugin Template provided by RTXI
 * Grupo de Neurocomputación Biológica (GNB), Universidad Autónoma de Madrid, 2018
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <default_gui_model.h>

class TriplePulseStimulator : public DefaultGUIModel
{

  Q_OBJECT

public:
  TriplePulseStimulator(void);
  virtual ~TriplePulseStimulator(void);

  void execute(void);
  void createGUI(DefaultGUIModel::variable_t*, int);
  void customizeGUI(void);
  void calculateDuration();

protected:
  virtual void update(DefaultGUIModel::update_flags_t);

private:
  double period;
  double count;
  double status;
  double time;
  double duration;

  double t1_ini;
  double t2_ini;
  double t1_max;
  double t2_max;
  double recovery;
  double step;
  double amplitude;

  double t1;
  double t2;
  double flag_pulse;

  void initParameters();

private slots:
  // these are custom functions that can also be connected to events
  // through the Qt API. they must be implemented in plugin_template.cpp

  void restart_button_event(void);
};
