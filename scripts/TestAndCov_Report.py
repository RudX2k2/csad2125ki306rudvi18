import os
import re
import webbrowser
from datetime import datetime

def parse_test_log(log_file):
    with open(log_file, 'r') as f:
        content = f.read()
    
    # Parse Qt test output
    totals_pattern = r'Totals: (\d+) passed, (\d+) failed, (\d+) skipped'
    match = re.search(totals_pattern, content)
    
    if match:
        passed, failed, skipped = map(int, match.groups())
        total = passed + failed + skipped
        percentage = (passed / total) * 100 if total > 0 else 0
        
        # Get individual test results
        test_results = []
        for line in content.split('\n'):
            if 'PASS' in line or 'FAIL' in line:
                result = {'status': 'PASS' if 'PASS' in line else 'FAIL'}
                test_name = re.search(r'(?:PASS|FAIL)\s+: (.+?)\(', line)
                if test_name:
                    result['name'] = test_name.group(1)
                    test_results.append(result)
        
        return {
            'total': total,
            'passed': passed,
            'failed': failed,
            'percentage': percentage,
            'test_results': test_results
        }
    return None

def parse_coverage_info(info_file):
    with open(info_file, 'r') as f:
        content = f.read()
    
    line_pattern = r'lines\.*:\s*(\d+\.\d+)%'
    func_pattern = r'functions\.*:\s*(\d+\.\d+)%'
    
    line_match = re.search(line_pattern, content)
    func_match = re.search(func_pattern, content)
    
    return {
        'line_coverage': float(line_match.group(1)) if line_match else 0,
        'function_coverage': float(func_match.group(1)) if func_match else 0
    }

def get_latest_timestamp(directory):
    files = [f for f in os.listdir(directory) if 'test_results_' in f and f.endswith('.log')]
    return max([f.replace('test_results_', '').replace('.log', '') for f in files])

def generate_report(deploy_dir):
    test_results_dir = os.path.join(deploy_dir, 'test_results')
    timestamp = get_latest_timestamp(test_results_dir)
    
    test_log = os.path.join(test_results_dir, f'test_results_{timestamp}.log')
    coverage_info = os.path.join(test_results_dir, f'coverage_{timestamp}.info')
    
    test_results = parse_test_log(test_log)
    coverage_results = parse_coverage_info(coverage_info)

    html_content = f'''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Test and Coverage Report</title>
        <style>
            body {{ font-family: Arial, sans-serif; margin: 40px; }}
            .summary {{ background: #f5f5f5; padding: 20px; border-radius: 5px; margin-bottom: 20px; }}
            .metric {{ font-size: 1.2em; margin: 10px 0; }}
            .good {{ color: #28a745; font-weight: bold; }}
            .warning {{ color: #ffc107; font-weight: bold; }}
            .danger {{ color: #dc3545; font-weight: bold; }}
            .coverage-section {{ margin-top: 30px; }}
            .test-details {{ margin-top: 20px; }}
            .test-case {{ padding: 10px; border-left: 4px solid #ddd; margin: 5px 0; }}
            .pass {{ border-color: #28a745; }}
            .fail {{ border-color: #dc3545; }}
            iframe {{ border: 1px solid #ddd; width: 100%; height: 800px; }}
        </style>
    </head>
    <body>
        <h1>Test and Coverage Report</h1>
        <div class="summary">
            <h2>Test Summary</h2>
            <p>Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>
            <div class="metric">
                Total Tests: {test_results['total']}
            </div>
            <div class="metric {('good' if test_results['percentage'] > 90 else 'warning' if test_results['percentage'] > 70 else 'danger')}">
                Passed: {test_results['passed']} ({test_results['percentage']:.1f}%)
            </div>
            <div class="metric {('danger' if test_results['failed'] > 0 else 'good')}">
                Failed: {test_results['failed']}
            </div>
            
            <div class="test-details">
                <h3>Test Details</h3>
                {''.join([f"""
                    <div class="test-case {'pass' if r['status'] == 'PASS' else 'fail'}">
                        {r['name']}: {r['status']}
                    </div>
                """ for r in test_results['test_results']])}
            </div>
        </div>
        
        <div class="coverage-section">
            <h2>Detailed Coverage Report</h2>
            <iframe src="coverage_{timestamp}/index.html"></iframe>
        </div>
    </body>
    </html>
    '''
    
    output_path = os.path.join(test_results_dir, f'combined_report_{timestamp}.html')
    with open(output_path, 'w') as f:
        f.write(html_content)
    
    return output_path

if __name__ == '__main__':
    deploy_dir = '/home/rud/csad2125ki406rudvi18/deploy'
    report_path = generate_report(deploy_dir)
    print(f"Report generated at: {report_path}")
    RdFile = webbrowser.open(report_path)  #Full path to your file

