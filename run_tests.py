#!/usr/bin/env python
import os, subprocess, time, glob, argparse

def run_test(test_name):
	global t_pass, t_skip, t_fail, t_err, t_total_time, args
	if args.dry_run:
		print('   Test - {0:<50}        \033[33m [Canceled] \033[0m ({1:.5f} sec.)'.format(test_name, 0))
		t_skip += 1
		return

	start_time = time.time()
	try:
		if args.godot_executable_wrapper != None:
			prx = subprocess.Popen([args.godot_executable_wrapper, args.godot_executable, '--script', 'unit_tests/' + test_name, '--path', args.target_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		else:
			prx = subprocess.Popen([args.godot_executable, '--script', 'unit_tests/' + test_name, '--path', args.target_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		log = prx.communicate()
		status = prx.returncode
	except Exception as e:
		log = '{0}'.format(str(e))
		status = -1
	run_time = (time.time() - start_time)
	t_total_time += run_time
	if (status == 10):
		print('> Test - {0:<50}        \033[32m [Passed  ] \033[0m ({1:.5f} sec.)'.format(test_name, run_time))
		t_pass += 1
	elif (status == 77):
		print('> Test - {0:<50}        \033[33m [Skipped ] \033[0m ({1:.5f} sec.)'.format(test_name, run_time))
		t_skip += 1
	elif (status == 100):
		print('> Test - {0:<50}        \033[31m [Failed  ] \033[0m ({1:.5f} sec.)'.format(test_name, run_time))
		t_fail += 1
	else:
		print('> Test - {0:<50}        \033[31m [Error   ] \033[0m ({1:.5f} sec.)'.format(test_name, run_time))
		t_err += 1
	if (args.verbose == 2):
		log_lines = log[0].splitlines(False)
		for l in log_lines:
			print(' >>[0]>> {}'.format(l))
	if ((status != 10) and (status != 77)) or (args.verbose >= 1):
		log_lines = log[1].splitlines(False)
		for l in log_lines:
			print(' >>[1]>> {}'.format(l))

def show_results():
	t_total = t_err + t_pass + t_skip + t_fail

	print('')
	print('Passed:  \033[32m {:>5} \033[0m'.format(t_pass) )
	print('Failed:  \033[31m {:>5} \033[0m'.format(t_fail) )
	print('Skipped: \033[33m {:>5} \033[0m'.format(t_skip) )
	if (t_err != 0):
		print('Error:   \033[31m {:>5} \033[0m'.format(t_err) )
	print('--------------------------------------')
	print('Total:    {:>5} \033[0m ({:.5f} sec.)'.format(t_total, t_total_time) )
	print('')

t_pass = 0
t_skip = 0
t_fail = 0
t_err = 0
t_total_time = 0.0

parser = argparse.ArgumentParser(description='Runs GDScript unit tests.')
parser.add_argument('--target-path', help='Path to target directory.', default='./libgodot_sqlite3', metavar='{target}')
parser.add_argument('--godot-executable', help='Path to the Godot executable.', default='godot', metavar='{executable}')
parser.add_argument('--godot-executable-wrapper', help='Path to the Godot executable wrapper (e.g. Wine).', default=None, metavar='{exe_wrapper}')
parser.add_argument('--dry-run', help='Instead of running the test only list them.', action='store_const', const=True, default=False)
parser.add_argument('--fail-fast', help='Interrupt on first failed test.', action='store_const', const=True, default=False)
parser.add_argument('--verbose', type=int, help='Enable verbose output (0 - output STDERR of failed tests, 1 - output STDERR of all test, 2 - output STDERR and STDOUT of all tests).', metavar='{level}', default=0)
args = parser.parse_args()

list = glob.glob(args.target_path + '/unit_tests/*.gd')
list.sort()

print('')
print('Running {} tests...'.format(len(list)))
print('')

for f in list:
	run_test(os.path.basename(f))
	if args.fail_fast and ((t_err > 0) or (t_fail > 0)):
		print('Job interrupted...')
		break

show_results()
