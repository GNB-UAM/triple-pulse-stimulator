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

#include "triple-pulse-stimulator.h"
#include <iostream>
#include <main_window.h>

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new TriplePulseStimulator();
}

static DefaultGUIModel::variable_t vars[] = {
    { "Pulse (V)", "Pulse generated", DefaultGUIModel::OUTPUT,},
    { "Event", "Identificator of the pulse generated (1, 2 or 3)", DefaultGUIModel::OUTPUT,},
    { "T1 initial (s)", "Initial time for the T1 interval", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "T2 initial (s)", "Initial time for the T2 interval", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "T1 max (s)", "Maximum time for the T1 interval", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "T2 max (s)", "Maximum time for the T2 interval", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "Recovery time (s)", "Recovery time between groups of pulses", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "Step (s)", "Time step for the intervals", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "Amplitude (V)", "Generated pulses amplitude", DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,},
    { "Status (Finished == 1)", "When the trial ends, status is 1", DefaultGUIModel::STATE,},
    { "Trial duration (s)", "Trial duration", DefaultGUIModel::STATE,},
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

TriplePulseStimulator::TriplePulseStimulator(void)
  : DefaultGUIModel("Triple Pulse Stimulator", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>Triple Pulse Stimulator:</b><br>Generates groups of three pulses with separation T1 between pulses 1 and 2, and T2 between pulses 2 and 3. T1 and T2 values will be mapped from their initial values to their maximum ones, leaving an specific recovery time between each group.</p>");
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  initParameters();
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

TriplePulseStimulator::~TriplePulseStimulator(void)
{
}

void
TriplePulseStimulator::calculateDuration(void)
{
	double total = 0.0;
	double i, j;

	for (i = t1_ini; i < t1_max; i += step) {
		for (j = t2_ini; j < t2_max; j += step) {
			total += i + j + recovery;
		}
	}

	duration = total;
}

void
TriplePulseStimulator::execute(void)
{
    time = count * RT::System::getInstance()->getPeriod() * 1e-9; //s

    if (flag_pulse == 0 && time >= recovery) {
        output(0) = amplitude;
        output(1) = 1;
        flag_pulse = 1;
    } else if (flag_pulse == 1 && time >= (recovery + t1)) {
        output(0) = amplitude;
        output(1) = 2;
        flag_pulse = 2;
    } else if (flag_pulse == 2 && time >= (recovery + t1 + t2)) {
        output(0) = amplitude;
        output(1) = 3;
        flag_pulse = 0;
        count = 0;

        if (t1 < t1_max) {
            if (t2 < t2_max) {
                t2 += step;
            } else {
                t2 = t2_ini;
                t1 += step;
            }
        } else {
            flag_pulse = 4;
        }
    } else if (flag_pulse == 4){
        status = 1;
        output(0) = 0;
        output(1) = 0;
    } else {
        output(0) = 0;
        output(1) = 0;
    }

    count++;


    return;
}

void
TriplePulseStimulator::initParameters(void)
{
  t1 = 0;
  t2 = 0;


  t1_max = 0;
  t2_max = 0;
  t1_ini = 0;
  t2_ini = 0;
  recovery = 0;
  step = 0;

  count = recovery / RT::System::getInstance()->getPeriod() * 1e-9;
  flag_pulse = 0;
  status = 0;
  time = 0;
  amplitude = 0;

  duration = 0;
}

void
TriplePulseStimulator::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      setParameter("T1 max (s)", t1_max);
      setParameter("T2 max (s)", t2_max);
      setParameter("T1 initial (s)", t1_ini);
      setParameter("T2 initial (s)", t2_ini);
      setParameter("Recovery time (s)", recovery);
      setParameter("Step (s)", recovery);
      setParameter("Amplitude (V)", amplitude);
      setState("Status (Finished == 1)", status);
      setState("Trial duration (s)", duration);
      break;

    case MODIFY:
      t1_ini = getParameter("T1 initial (s)").toDouble();
      t2_ini = getParameter("T2 initial (s)").toDouble();
      t1 = t1_ini;
      t2 = t2_ini;
      t1_max = getParameter("T1 max (s)").toDouble();
      t2_max = getParameter("T2 max (s)").toDouble();
      recovery = getParameter("Recovery time (s)").toDouble();
      step = getParameter("Step (s)").toDouble();
      amplitude = getParameter("Amplitude (V)").toDouble();
      flag_pulse = 0;
      status = 0;
      count = recovery / RT::System::getInstance()->getPeriod() * 1e-9;

      calculateDuration();
      break;

    case UNPAUSE:

      break;

    case PAUSE:
      break;

    case PERIOD:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    default:
      break;
  }
}

void
TriplePulseStimulator::customizeGUI(void)
{
  QGridLayout* customlayout = DefaultGUIModel::getLayout();

  QGroupBox* button_group = new QGroupBox;

  QPushButton* abutton = new QPushButton("Restart");
  QHBoxLayout* button_layout = new QHBoxLayout;
  button_group->setLayout(button_layout);
  button_layout->addWidget(abutton);
  QObject::connect(abutton, SIGNAL(clicked()), this, SLOT(restart_button_event()));

  customlayout->addWidget(button_group, 0, 0);
  setLayout(customlayout);
}

// functions designated as Qt slots are implemented as regular C++ functions
void
TriplePulseStimulator::restart_button_event(void)
{
    status = 0;
    flag_pulse = 0;
    count = recovery / RT::System::getInstance()->getPeriod() * 1e-9;
    t1 = t1_ini;
    t2 = t2_ini;
}
