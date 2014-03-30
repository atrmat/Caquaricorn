clear all;
path='J:\2014_czh_thesis\data\Kmeans_80w\Kmeans_80w_µÚ6Ê±¶Î\canopy+Kmeans\3-20-6\parse\cluster_distribute_';
files=[24 6 2 14 19 5 13 18 17];
fileType='.csv';
plotType=['.','h','o','x','*','s','d','v','^','<','>','p','+']; % 13 plot types
plotNum=length(plotType)-1;
colorType=['b','r','g','m','c','k','y']; % 8 color types,  ,'w',
colorNum=length(colorType)-1;
N=length(files);
for i=1:1:N
    fileNum = num2str(files(i)); % uncontinus num
    filePath=[path,fileNum,fileType];
    sgc_exist = exist(filePath, 'file'); 
    if sgc_exist ~= 2 %% if the file doesn't exist, continue loop
        continue;
    end
    result=importdata(filePath);
    [a,n]=hist(result,25);
    pt = mod(i,plotNum)+1;
    ct = mod(i,colorNum)+1;
    subplot(3,3,i);
    if length(find(a,sum(a))) == 1
        plot([n(find(a,sum(a))) n(find(a,sum(a)))],[0 1],['-',colorType(ct),plotType(pt)]);
    else
        plot(n,a/sum(a),['-',colorType(ct),plotType(pt)]);
    end
%     axis([0 max(n)+0.05 0 max(a)/sum(a)+0.075]);
    axis([0 1 0 max(a)/sum(a)+0.3]);
    grid on;
    xlabel('distance');
    ylabel('probability');
    legend(['G',num2str(i)],'Location','Best');
end
    saveas(gcf,[path,'all clusters','.png']);
    close all;