using IRHeaterHardwareLibrary;
using IRHeaterHardwareLibrary.Data;
using System;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using IRHeaterHardwareLibrary.Entities;

namespace IR_Heater
{
    public partial class MainFrm : Form
    {
        readonly HardwareLibrary _hardwareLibrary = new HardwareLibrary();

        private bool _connected;

        private Configuration configuration;

        public MainFrm()
        {
            InitializeComponent();
        }

        private void MainFrm_Load(object sender, EventArgs e)
        {
            Connect();
        }

        private void startBtn_Click(object sender, EventArgs e)
        {
            SetTargetAsync((ModeEnum) comboBox1.SelectedIndex, new[]
            {
                (float)Temp1UpDown.Value,
                (float)Temp2UpDown.Value,
                (float)Temp3UpDown.Value,
                (float)Temp4UpDown.Value,
                (float)Temp5UpDown.Value
            });

            chart.Series.Clear();
            foreach (string sensorName in configuration.Sensors)
            {
                chart.Series.Add(sensorName);
            }

            foreach (Series series in chart.Series)
            {
                series.ChartType = SeriesChartType.Spline;
            }
            timer1.Enabled = true;
        }
        

        private void timer1_Tick(object sender, EventArgs e)
        {
            WriteChart();
        }

        private void checkBox9_CheckedChanged(object sender, EventArgs e)
        {
            chart.Series[0].Enabled = checkBox9.Checked;
        }

        private void checkBox10_CheckedChanged(object sender, EventArgs e)
        {
            chart.Series[1].Enabled = checkBox10.Checked;
        }

        private void checkBox11_CheckedChanged(object sender, EventArgs e)
        {
            chart.Series[2].Enabled = checkBox11.Checked;
        }

        private void checkBox12_CheckedChanged(object sender, EventArgs e)
        {
            chart.Series[3].Enabled = checkBox12.Checked;
        }

        private void checkBox13_CheckedChanged(object sender, EventArgs e)
        {
            chart.Series[4].Enabled = checkBox12.Checked;
        }

        async void Connect()
        {
            ErrorEnum connectError = await _hardwareLibrary.ConnectAsync(null, true);

            if (connectError == ErrorEnum.NoError)
            {
                _connected = true;
                startBtn.Enabled = true;
                configuration = _hardwareLibrary.GetConfiguration();
                NameLabel.Text = configuration.MainName;
            }

            switch (connectError)
            {
                case ErrorEnum.NoError:
                    statusLabel.Text = "Подключено";
                    break;
                case ErrorEnum.NoPorts:
                    statusLabel.Text = "В системе нет COM портов";
                    break;
                case ErrorEnum.NoSlave:
                    statusLabel.Text = "Устройство не отвечает";
                    break;
                case ErrorEnum.PortIoError:
                    statusLabel.Text = "IO Error";
                    break;
                case ErrorEnum.PortNotFound:
                    statusLabel.Text = "Устройство не найдено ни на одном из портов";
                    break;
                case ErrorEnum.PortTimeoutError:
                    statusLabel.Text = "PortTimeoutError";
                    break;
                case ErrorEnum.PortUnauthorizedAccessError:
                    statusLabel.Text = "PortUnauthorizedAccessError";
                    break;
                default:
                    statusLabel.Text = "Неизвестная ошибка";
                    break;
            }
        }

        async void SetTargetAsync(ModeEnum mode, float[] temperatures)
        {
            await _hardwareLibrary.SetTargetAsync(mode, temperatures);
        }

        async void WriteChart()
        {
            try
            {
                Temperatures currentTemp = await _hardwareLibrary.GetTemperaturesAsync();
                for (int i = 0; i < currentTemp.temperature.Length; i++)
                {
                    if (chart.Series.Count > i)
                        chart.Series[i].Points.AddY(currentTemp.temperature[i]);
                }
            }
            catch
            {

            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SetTargetAsync(0, new float[] {0, 0, 0, 0, 0});
            timer1.Enabled = false;
        }

        private void chart_Click(object sender, EventArgs e)
        {

        }
    }
}
